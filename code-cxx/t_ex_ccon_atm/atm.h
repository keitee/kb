#ifndef ATM_H_
#define ATM_H_

#include <iostream>
#include <thread>
#include <future>
#include <list>
#include <mutex>
#include <queue>
#include <stack>
#include <exception>
#include <boost/thread/shared_mutex.hpp>

#include "events.h"
#include "message.h"


// ={=========================================================================
// listing_c.9.cpp

class interface_machine
{
    messaging::receiver incoming;
    std::mutex iom;
public:
    void done()
    {
        get_sender().send(messaging::close_queue());
    }
    void run()
    {
        try
        {
            for(;;)
            {
                incoming.wait()
                    .handle<issue_money>(
                        [&](issue_money const& msg)
                        {
                            (void)msg;
                            {
                                std::lock_guard<std::mutex> lk(iom);
                                std::cout<<"Issuing "
                                         <<msg.amount<<std::endl;
                            }
                        }
                        )
                    .handle<display_insufficient_funds>(
                        [&](display_insufficient_funds const& msg)
                        {
                            (void)msg;
                            {
                                std::lock_guard<std::mutex> lk(iom);
                                std::cout<<"Insufficient funds"<<std::endl;
                            }
                        }
                        )
                    .handle<display_enter_pin>(
                        [&](display_enter_pin const& msg)
                        {
                            (void)msg;
                            {
                                std::lock_guard<std::mutex> lk(iom);
                                std::cout
                                    <<"Please enter your PIN (0-9)"
                                    <<std::endl;
                            }
                        }
                        )
                    .handle<display_enter_card>(
                        [&](display_enter_card const& msg)
                        {
                            (void)msg;
                            {
                                std::lock_guard<std::mutex> lk(iom);
                                std::cout<<"Please enter your card (I)"
                                         <<std::endl;
                            }
                        }
                        )
                    .handle<display_balance>(
                        [&](display_balance const& msg)
                        {
                            (void)msg;
                            {
                                std::lock_guard<std::mutex> lk(iom);
                                std::cout
                                    <<"The balance of your account is "
                                    <<msg.amount<<std::endl;
                            }
                        }
                        )
                    .handle<display_withdrawal_options>(
                        [&](display_withdrawal_options const& msg)
                        {
                            (void)msg;
                            {
                                std::lock_guard<std::mutex> lk(iom);
                                std::cout<<"Withdraw 50? (w)"<<std::endl;
                                std::cout<<"Display Balance? (b)"
                                         <<std::endl;
                                std::cout<<"Cancel? (c)"<<std::endl;
                            }
                        }
                        )
                    .handle<display_withdrawal_cancelled>(
                        [&](display_withdrawal_cancelled const& msg)
                        {
                            (void)msg;
                            {
                                std::lock_guard<std::mutex> lk(iom);
                                std::cout<<"Withdrawal cancelled"
                                         <<std::endl;
                            }
                        }
                        )
                    .handle<display_pin_incorrect_message>(
                        [&](display_pin_incorrect_message const& msg)
                        {
                            (void)msg;
                            {
                                std::lock_guard<std::mutex> lk(iom);
                                std::cout<<"PIN incorrect"<<std::endl;
                            }
                        }
                        )
                    .handle<eject_card>(
                        [&](eject_card const& msg)
                        {
                            (void)msg;
                            {
                                std::lock_guard<std::mutex> lk(iom);
                                std::cout<<"Ejecting card"<<std::endl;
                            }
                        }
                        );
            }
        }
        catch(messaging::close_queue&)
        {
        }
    }
    messaging::sender get_sender()
    {
        return incoming;
    }
};

// ={=========================================================================
// listing_c.8.cpp

class bank_machine
{
  messaging::receiver incoming;
  unsigned balance;
  public:
  bank_machine():
    balance(199)
  {}
  void done()
  {
    get_sender().send(messaging::close_queue());
  }
  void run()
  {
    try
    {
      for(;;)
      {
        incoming.wait()
          .handle<verify_pin>
          (
            [&](verify_pin const& msg)
            {
              if(msg.pin=="1937")
              {
                msg.atm_queue.send(pin_verified());
              }
              else
              {
                msg.atm_queue.send(pin_incorrect());
              }
            }
          )
          .handle<withdraw>
          (
            [&](withdraw const& msg)
            {
              if(balance>=msg.amount)
              {
                msg.atm_queue.send(withdraw_ok());
                balance-=msg.amount;
              }
              else
              {
                msg.atm_queue.send(withdraw_denied());
              }
            }
          )
          .handle<get_balance>
          (
            [&](get_balance const& msg)
            {
              msg.atm_queue.send(::balance(balance));
            }
          )
          .handle<withdrawal_processed>
          (
            [&](withdrawal_processed const& msg)
            {
              (void)msg;
            }
          )
          .handle<cancel_withdrawal>
          (
            [&](cancel_withdrawal const& msg)
            {
              (void)msg;
            }
          );
      }
    }
    catch(messaging::close_queue const&)
    {
    }
  }
  messaging::sender get_sender()
  {
    return incoming;
  }
};


// ={=========================================================================

class atm
{
  private:
    messaging::receiver incoming_;
    messaging::sender bank_;
    messaging::sender interface_hardware_;

    void (atm::*state)();

    std::string account_;
    std::string pin_;
    unsigned withdraw_amount_;

    void waiting_for_card()
    {
      // interface_hardware.send(display_enter_card());
      //
      // state waitinf_for_card -> event card_inserted -> state getting_pin
      //                              -> eaf, lambda

      incoming_.wait()
        .handle<card_inserted>
        (
         [&](const card_inserted &msg) 
         {
           std::cout << "atm::waiting_for_card::card_inserted" << std::endl;
           account_ = msg.account;
           pin_ = "";
           // interface_hardware_.send(display_enter_pin());
           state = &atm::getting_pin;
         }
        );
    }

    void getting_pin()
    {
      incoming_.wait()
        .handle<digit_pressed>
        (
          [&](const digit_pressed &msg)
          {
            (void)msg;
            unsigned const pin_length = 4;
            pin_ += msg.digit;
            std::cout << "atm::getting_pin::digit_pressed: " << pin_ << std::endl;
            if (pin_.length() == pin_length)
            {
              // bank.send(verify_pin(account, pin, incoming));
              state = &atm::verifying_pin;
            }
          }
        )
        .handle<clear_last_pressed>
        (
          [&](const clear_last_pressed &msg)
          {
            (void)msg;
            std::cout << "atm::getting_pin::clear_last_pressed" << std::endl;
            if (!pin_.empty())
            {
              pin_.pop_back();
            }
          }
        )
        .handle<cancel_pressed>
        (
          [&](const cancel_pressed &msg)
          {
            (void)msg;
            std::cout << "atm::getting_pin::cancel_pressed" << std::endl;
            state = &atm::done_processing;
          }
        );
    }

    // expect pin_* message from back fsm.
    void verifying_pin()
    {
      incoming_.wait()
        .handle<pin_verified>
        (
          [&](const pin_verified &msg)
          {
            (void)msg;
            std::cout << "atm::verify_pin::pin_verified" << std::endl;
            state = &atm::wait_for_action;
          }
        )
        .handle<pin_incorrect>
        (
          [&](const pin_incorrect &msg)
          {
            (void)msg;
            std::cout << "atm::verify_pin::pin_incorrect" << std::endl;
            // interface_hardware_.send(display_pin_incorrect_message());
            state = &atm::done_processing;
          }
        )
        .handle<cancel_pressed>
        (
          [&](const cancel_pressed &msg)
          {
            (void)msg;
            std::cout << "atm::verify_pin::cancel_pressed" << std::endl;
            state = &atm::done_processing;
          }
        );
    }

    void wait_for_action()
    {
      // interface_hardware_.send(display_withdrawal_options());
      incoming_.wait()
        .handle<withdraw_pressed>
        (
          [&](const withdraw_pressed &msg)
          {
            withdraw_amount_ = msg.amount;
            std::cout << "atm::wait_for_action::withdraw_pressed: " << withdraw_amount_ << std::endl;
            // bank_.send(withdraw(account_, msg.account, incoming_));
            state = &atm::process_withdrawal;
          }
        )
        .handle<balance_pressed>
        (
          [&](const balance_pressed &msg)
          {
            (void)msg;
            std::cout << "atm::wait_for_action::balance_pressed" << std::endl;
            // bank_.send(get_balance(account_, incoming_));
            state = &atm::process_balance;
          }
        )
        .handle<cancel_pressed>
        (
          [&](const cancel_pressed &msg)
          {
            (void)msg;
            std::cout << "atm::wait_for_action::cancel_pressed" << std::endl;
            state = &atm::done_processing;
          }
        );
    }

    void process_balance()
    {
      incoming_.wait()
        .handle<balance>
        (
          [&](const balance &msg)
          {
            (void)msg;
            std::cout << "atm::process_balance::balance" << std::endl;
            interface_hardware_.send(display_balance(msg.amount));
            state = &atm::wait_for_action;
          }
        )
        .handle<cancel_pressed>
        (
          [&](const cancel_pressed &msg)
          {
            (void)msg;
            std::cout << "atm::process_balance::cancel_pressed" << std::endl;
            state = &atm::done_processing;
          }
        );
    }

    void process_withdrawal()
    {
      incoming_.wait()
        .handle<withdraw_ok>
        (
          [&](const withdraw_ok &msg)
          {
            (void)msg;
            std::cout << "atm::process_withdrawal::withdraw_ok" << std::endl;
            // interface_hardware_.send(issue_money(withdrawal_amount));
            // bank_.send(withdrawal_processed(account,withdrawal_amount));
            state = &atm::done_processing;
          }
        )
        .handle<withdraw_denied>
        (
          [&](const withdraw_denied &msg)
          {
            (void)msg;
            std::cout << "atm::process_withdrawal::withdraw_denied" << std::endl;
            // interface_hardware_.send(display_insufficient_funds());
            state = &atm::done_processing;
          }
        )
        .handle<cancel_pressed>
        (
          [&](const cancel_pressed &msg)
          {
            (void)msg;
            std::cout << "atm::process_withdrawal::cancel_pressed" << std::endl;
            // bank_.send(cancel_withdrawal(account,withdrawal_amount));
            // interface_hardware_.send(display_withdrawal_cancelled());
            state = &atm::done_processing;
          }
        );
    }

    void done_processing()
    {
      // interface_hardware_.send(eject_card());
      state = &atm::getting_pin;
    }

  public:
    atm(messaging::sender bank, messaging::sender interface_hardware):
      bank_(bank), interface_hardware_(interface_hardware) {}

    atm(const atm &) = delete;
    atm &operator=(const atm &) = delete;

    // send a message to self
    void done()
    {
      get_sender().send(messaging::close_queue());
    }

    void run()
    {
      state = &atm::waiting_for_card;

      std::cout << "atm::run::for" << std::endl;

      try
      {
        for (;;)
        {
          (this->*state)();
        }
      }
      catch(messaging::close_queue const &)
      {
        // note: what would happen when catch exception but do nothing?
      }
    }

    // Whereas a sender just references a message queue, a receiver owns it. You
    // can obtain a sender that references the queue by using the implicit
    // conversion.
    //
    // receive incoming is converted to sender which is temporary.
    //
    // note: this is an interface to expose q to other class and used to set up
    // connection each other
    //
    // atm machine(bank.get_sender(), intarface_hardware.get_sender());
    //
    // atm machine keeps this sender copies to send messages to them.

    // note: cxx-conversion-op which convert receiver to sender
    messaging::sender get_sender()
    {
      return incoming_;
    }
};

#endif // ATM_H_
