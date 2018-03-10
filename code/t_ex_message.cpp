#include <iostream>
#include <set>
#include <vector>
#include <string>

// TN:
// * Do not use dynamic allocations but use pointers.
//
// * Do not use dynamic allocation, but use copy controls since her "copy" has
//   different meaning
//
//
// Sketch its operations:
//
// * Folder has pointers to Messages that is has and Messages has pointers to
// Folders it belongs.
// 
// * Message class provide save() and remove() operations to add or remove a
// Message from a specified folder. To create a new Message, specify the
// contents of the message and call save() to put a message to a folder.
//
// * To copy a message, the copy and the original will be a 'distinct' Message
// and shall be in the same 'set' of Folders. Thus copy the contents and to make
// the new to appear in the same Folders as the original, must update Folders to
// add the new message into each Folder. ADD. copy-ctor
//
// * To destroy a Message, remove it from the Folders. REMOVE. dtor.
//
// * To assign a Message, will replace the contents and remove Message from lhs
// Folder-set and add that distinct Message to rhs Folder-set. REMOVE and ADD.
// copy-assign.
//
// *cpp-remember*
//  The copy assign often does the same work as is needed in the copy ctor and
//  dtor. In this example, REMOVE and ADD. In such case the 'common' work should
//  be put in private utility functions.
//
// {
//     string s1("contents1");
// 
//     // all new messages, no copies yet
//     Message m1(s1);
// 
//     Folder f1;
// 
//     m1.save(f1); m3.save(f1); m5.save(f1);
// 
//     m1.debug_print();
//     f2.debug_print();
// 
//     // create some copies
//     Message c1(m1);
// 
//     m1.debug_print();
//     f2.debug_print();
// 
//     // now some assignments
//     m2 = m3;
// 
//     m1.debug_print();
//     f2.debug_print();
// 
//     // finally, self-assignment
//     m2 = m2;
// 
//     m1.debug_print();
//     f2.debug_print();
// }

class Message;

class Folder
{
    public:
        // def ctor ok
        Folder() = default;

        // copy ctor
        Folder(const Folder &f);

        // copy assign
        Folder &operator=(const Folder &f);

        ~Folder();

        // for user/clients
        void save(Message &m);
        void remove(Message &m);

        // for Message
        void addMsg(Message *m) { messages_.insert(m); }
        void removeMsg(Message *m) { messages_.erase(m); }

        // for debug
        void debug_print();

    private:
        std::set<Message *> messages_{};
};


class Message
{
    // *cxx-friend-member*
    friend void Folder::debug_print();
    friend void swap(Message &lhs, Message &rhs);

    public:

        // def ctor
        explicit Message(const std::string &contents = ""): contents_(contents) {}

        // copy ctor
        Message(const Message &rhs): 
            contents_(rhs.contents_), folders_(rhs.folders_)
        {
            // add_to_folders(). add lhs to folders
            for (auto &folder : folders_)
                folder->addMsg(this);
        }
        
        // copy assign
        //
        // TN:
        // *cxx-cxcept-safety*
        // it say "handle self-assignment by removing pointers before inserting
        // them" but that's not since clear() clears and remove it effectively. 
        //
        // if comment out clear() below, works file for self assign.
        // adding steps.

        Message &operator=(const Message &rhs)
        {
            // remove_from_folders(). remove lhs from folders. duplicate
            for (auto &folder : folders_)
                folder->removeMsg(this);

            // what if not call this?
            folders_.clear();

            contents_ = rhs.contents_;
            folders_ = rhs.folders_;

            // add_to_folders(). add lhs to folders. duplicate
            for (auto &folder : folders_)
                folder->addMsg(this);

            return *this;
        }

        // move ctor
        Message(Message &&rhs):
            contents_(std::move(rhs.contents_))
        {
            // move_folders(). use set move assign. duplicate
            folders_ = std::move(rhs.folders_);

            // however, do more to build "move" meaning in this class
            for (auto &folder : folders_)
            {
                folder->removeMsg(&rhs);        // pointer
                folder->addMsg(this);
            }

            // ensure that destrying m is `harmless`
            rhs.folders_.clear();
        }

        // move assign
        Message &operator=(Message &&rhs)
        {
            // cxx-self-assign direct check
            if ( &rhs != this )
            {
                // remove_from_folders(). remove lhs from folders. duplicate
                for (auto &folder : folders_)
                    folder->removeMsg(this);

                folders_.clear();

                contents_ = std::move(rhs.contents_);

                // move_folders()
                folders_ = std::move(rhs.folders_);

                // however, do more than moving members to build "move" meaning
                // in this class

                for (auto &folder : folders_)
                {
                    folder->removeMsg(&rhs);        // pointer
                    folder->addMsg(this);
                }
            }

            return *this;
        }

        ~Message()
        {
            // remove_from_folders(). remove lhs from folders. duplicate
            for (auto &folder : folders_)
                folder->removeMsg(this);

            folders_.clear();
        }

        // for user/client
        // add a message to the folder
        void save(Folder &f)
        {
            // add the folder to this message
            folders_.insert(&f);

            // add this message to the folder
            f.addMsg(this);
        }

        // remove a message from the folder
        void remove(Folder &f)
        {
            // remove the folder from this message
            folders_.erase(&f);

            // remove this message from the folder
            f.removeMsg(this);
        }

        void addFldr(Folder *f) { folders_.insert(f); }
        void removeFldr(Folder *f) { folders_.erase(f); }

        void debug_print()
        {
            std::cout << "message: " << contents_ << std::endl;
            std::cout << "appears in " << folders_.size() << " folders" << std::endl;
        }

    private:
        // *cxx-in-class-init*
        std::string contents_{};
        std::set<Folder*> folders_{};
};


// define them here due to incomplete type issue

void swap(Message &lhs, Message &rhs)
{
	using std::swap;  // not strictly needed in this case, but good habit

	// remove pointers to each Message from their (original) respective Folders
	for (auto f: lhs.folders_) 
		f->removeMsg(&lhs);
	for (auto f: rhs.folders_) 
		f->removeMsg(&rhs);

	// swap the contents and Folder pointer sets
	swap(lhs.folders_, rhs.folders_);   // uses swap(set&, set&)
	swap(lhs.contents_, rhs.contents_); // swap(string&, string&)

	// add pointers to each Message to their (new) respective Folders
	for (auto f: lhs.folders_) 
		f->addMsg(&lhs);
	for (auto f: rhs.folders_) 
		f->addMsg(&rhs);
}


Folder::Folder(const Folder &f):
    messages_(f.messages_)
{
    // add_to_message()
    for (auto &mesg : messages_)
        mesg->addFldr(this);
}

Folder &Folder::operator=(const Folder &f)
{
    // remove_from_message()
    for (auto &mesg : messages_)
        mesg->removeFldr(this);

    messages_ = f.messages_;

    // add_to_messages()
    for (auto &mesg : messages_)
        mesg->addFldr(this);

    messages_.clear();

    return *this;
}

Folder::~Folder()
{
    // remove_from_message()
    for (auto &mesg : messages_)
        mesg->removeFldr(this);

    messages_.clear();
}

void Folder::save(Message &m)
{
    messages_.insert(&m);
    m.addFldr(this);
}

void Folder::remove(Message &m)
{
    messages_.erase(&m);
    m.removeFldr(this);
}

void Folder::debug_print()
{
    std::cout << "folder contents " << messages_.size() << " messages" << std::endl;
    size_t count = 0;
    for (auto m : messages_)
        std::cout << "message " << ++count << ": " << m->contents_ << std::endl;
}

int main()
{
    using namespace std;

    string s1("contents1");
    string s2("contents2");
    string s3("contents3");
    string s4("contents4");
    string s5("contents5");
    string s6("contents6");

    // all new messages, no copies yet
    //
    // message: contents1
    // appears in 2 folders
    // folder contents 4 messages
    // message 1: contents1
    // message 2: contents2
    // message 3: contents4
    // message 4: contents6

    Message m1(s1);
    Message m2(s2);
    Message m3(s3);
    Message m4(s4);
    Message m5(s5);
    Message m6(s6);

    Folder f1;
    Folder f2;

    m1.save(f1); m3.save(f1); m5.save(f1);
    m1.save(f2); m2.save(f2); m4.save(f2); m6.save(f2);

    m1.debug_print();
    f2.debug_print();

    cout << "------------" << endl;
    // create some copies
    //
    // message: contents1
    // appears in 2 folders
    //
    // f1, folder contents 4 messages
    // message 1: contents1
    // message 2: contents3
    // message 3: contents5
    // message 4: contents1, c1
    //
    // f2, folder contents 8 messages
    // message 1: contents1
    // message 2: contents2
    // message 3: contents4
    // message 4: contents6
    // message 5: contents1, c1
    // message 6: contents2
    // message 7: contents4
    // message 8: contents6
   
    Message c1(m1);
    Message c2(m2), c4(m4), c6(m6);

    m1.debug_print();
    f1.debug_print();
    f2.debug_print();

    cout << "------------" << endl;
    // now some assignments
    //
    // message: contents5
    // appears in 1 folders
    //
    // f1, folder contents 7 messages
    // message 1: contents5
    // message 2: contents3
    // message 3: contents3
    // message 4: contents5
    // message 5: contents5
    // message 6: contents3
    // message 7: contents1
    //
    // f2, folder contents 4 messages
    // message 1: contents1
    // message 2: contents2
    // message 3: contents4
    // message 4: contents6

    m2 = m3;
    m4 = m5;
    m6 = m3;
    m1 = m5;

    m1.debug_print();
    f1.debug_print();
    f2.debug_print();

    cout << "------------" << endl;
    // finally, self-assignment
    //
    // // m2(contents3) and m1(contents5) are in f1. since do not support self
    // assign, removes them effectively.
    //
    // message: contents5
    // appears in 1 folders
    // message: contents3
    // appears in 1 folders
    //
    // message: contents5
    // appears in 0 folders
    // message: contents3
    // appears in 0 folders
    //
    // folder contents 5 messages
    // message 1: contents3
    // message 2: contents5
    // message 3: contents5
    // message 4: contents3
    // message 5: contents1
    // folder contents 4 messages
    // message 1: contents1
    // message 2: contents2
    // message 3: contents4
    // message 4: contents6

    m1.debug_print();
    m2.debug_print();

    m2 = m2;
    m1 = m1;

    m1.debug_print();
    m2.debug_print();
    f1.debug_print();
    f2.debug_print();

    cout << "------------" << endl;
    // capacity: 0
    // capacity: 1
    // capacity: 2
    // capacity: 4
    // capacity: 4
    // capacity: 8
    
    // capacity: 0
    // capacity: 1
    // capacity: 2
    // capacity: 4
    // capacity: 4
    // capacity: 8
    vector<Message> vm;
    cout << "capacity: " << vm.capacity() << endl;
    vm.push_back(m1);

    cout << "capacity: " << vm.capacity() << endl;
    vm.push_back(m2);

    cout << "capacity: " << vm.capacity() << endl;
    vm.push_back(m3);

    cout << "capacity: " << vm.capacity() << endl;
    vm.push_back(m4);

    cout << "capacity: " << vm.capacity() << endl;
    vm.push_back(m5);

    cout << "capacity: " << vm.capacity() << endl;
    vm.push_back(m6);

    vector<Folder> vf;
    cout << "capacity: " << vf.capacity() << endl;
    vf.push_back(f1);

    cout << "capacity: " << vf.capacity() << endl;
    vf.push_back(f2);

    cout << "capacity: " << vf.capacity() << endl;
    vf.push_back(Folder(f1));

    cout << "capacity: " << vf.capacity() << endl;
    vf.push_back(Folder(f2));

    cout << "capacity: " << vf.capacity() << endl;
    vf.push_back(Folder());

    Folder f3;
    f3.save(m6);
    cout << "capacity: " << vf.capacity() << endl;
    vf.push_back(f3);

    return 0;
}

