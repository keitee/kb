#ifndef ANIMALS_HPP_INCLUDED
#define ANIMALS_HPP_INCLUDED

#include <string>
#include <regex>

// represent a person
class Contact
{
  public:
    Contact() {}
    Contact(const std::string &name, const std::string &phone)
      :name_(name)
    {
      SetPhone(phone);
    }

    std::string GetName() const { return name_; }
    std::string GetPhone() const { return phone_; }

    void SetName(const std::string &name) { name_ = name; }
    void SetPhone(const std::string &phone)
    {
      // to verify that phone has the form (ddd)ddd-dddd
      if (!std::regex_match(phone, pattern))
        throw std::runtime_error(std::string("bad phone number:") + phone);

      phone_ = phone;
    }

  private:
    std::string name_;
    std::string phone_;
    const std::string pattern{"\\([0-9]{3}\\)[0-9]{3}-[0-9]{4}"};
};

// operators
bool operator==(const Contact &lhs, const Contact &rhs)
{
  return lhs.GetName() == rhs.GetName() &&
    lhs.GetPhone() == rhs.GetPhone();
}

std::ostream &operator<<(std::ostream &os, const Contact &contact)
{
  os << contact.GetName() << " " << contact.GetPhone();
  return os;
}


// represent a animal
// * not use time but use string instead for brevity
class Animal
{
  public:
    Animal() {}

    Animal(const std::string &name, const std::string &species,
        const std::string &dob, const Contact &vet, const Contact &trainer)
      name_(name), species_(species), dob_(dob), vet_(vet), trainer_(trainer)
      {
      }

    std::string GetName() const { return name_; }
    std::string GetSpecies() const { return species_; }
    std::string GetDOB() const { return dob_; }
    Contact GetVet() const { return vet_; }
    Contact GetTrainer() const { return trainer_; }

    void SetName(const std::string &name) { name_ = name; }
    void SetSpecies(const std::string &species) { species_ = species; }
    void SetDOB(const std::string &dob) { dob_ = dob; }
    void SetVet(const Contact &vet) { vet_ = vet; }
    void SetTrainer(const Contact &trainer) { trainer_ = trainer; }

  private:
    std::string name_;
    std::string species_;
    std::string dob_;
    Contact vet_;
    Contact trainer_;
};

bool operator==(const Animal &lhs, const Animal &rhs)
{
  return lhs.GetName() == rhs.GetName() &&
    lhs.GetSpecies() == rhs.GetSpecies() &&
    lhs.GetDOB() == rhs.GetDOB() &&
    lhs.GetVet() == rhs.GetVet() &&
    lhs.GetTrainer() == rhs.GetTrainer();
}

std::ostream &operator<<(std::ostream &os, const Animal &animal)
{
  os << "Animal {\n"
    << " name    = " << animal.GetName() << ";\n"
    << " species = " << animal.GetSpecies() << ";\n"
    << " dob     = " << animal.GetDOB() << ";\n"
    << " vet     = " << animal.GetVet() << ";\n"
    << " trainer = " << animal.GetTrainer() << ";\n"
    << "}";

  return os;
}

#endif // ANIMALS_HPP_INCLUDED

