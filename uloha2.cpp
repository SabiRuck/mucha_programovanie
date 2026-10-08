#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

int lastId;

class Person 
{
protected:
    int id;
    string name;
    string email;
    string password;

public:

    Person(string n, string e, string p)
    {
        id = lastId +1;
        lastId = lastId +1;
        name = n;
        email = e;
        password = p;
    }

    virtual ~Person() = default;

    bool checkData(const string& e, const string& p) const 
    {
        return (email == e && password == p);
    }

    string getName() const { return name; }


    virtual void userActions(vector<Person*>& allUsers) = 0; 
};


class Student : public Person 
{
private:
    int drivesLeft = 17;

public:
    Student(string n, string e, string p) : Person(move(n), move(e), move(p)) {}

    int getDrives() const { return drivesLeft; }

    void userActions(vector<Person*>& allUsers) override 
    {
        while (true) 
        {
            cout << "(V)iew drives left?\n";
            cout << "(R)eserve driving lesson\n";
            cout << "(L)og out\n";
            cout << "Choice:";

            char choice;
            cin >> choice;

            if (choice == 'V') 
            {
                cout << "You have " << drivesLeft << " drives remaining.\n";
            } 
            else if (choice == 'R') 
            {
                if (drivesLeft > 0) 
                {
                    drivesLeft--;
                    cout << "Lesson reserved! Remaining drives: " << drivesLeft << "\n";
                } 
                else 
                {
                    cout << "No drives left!\n";
                }
            } 
            else if (choice == 'L') 
            {
                break;
            }
        }
    }
};


class Instructor : public Person 
{
public:
    Instructor(string n, string e, string p) : Person(move(n), move(e), move(p)) {}

    void userActions(vector<Person*>& allUsers) override 
    {
        while (true) 
        {
            cout << "(A)dd sriving lessons\n";
            cout << "(V)iew reserved drives\n";
            cout << "(L)og out\n";
            cout << "Choice: ";

            char choice;
            cin >> choice;

            if (choice == 'A') 
            {
                cout << "Slot added successfully.\n";
            } 
            else if (choice == 'V') 
            {
                cout << "Listing students...\n";
            } 
            else if (choice == 'L') 
            {
                break;
            }
        }
    }
};


class Admin : public Person 
{
private:
    void createNewUser(vector<Person*>& allUsers) 
    {
        char role;
        string name, surname, email, password;

        cout << "Account data:";
        cout << "Role (S-student,I-instructor,A-admin):";
        cin >> role;
        cout << "Enter first name, surename, email, password:\n";
        cin >> name >> surname >> email >> password;

        string fullName = name + " " + surname;
        Person* newUser = nullptr;

        switch (role) 
        {
            case 'S': newUser = new Student(fullName, email, password); break;
            case 'I': newUser = new Instructor(fullName, email, password); break;
            case 'A': newUser = new Admin(fullName, email, password); break;
            default:
                cout << "Invalid role choice!\n";
                return;
        }

        allUsers.push_back(newUser);
        cout << "Account created\n";
    }

public:
    Admin(string n, string e, string p) : Person(move(n), move(e), move(p)) {}

    void userActions(vector<Person*>& allUsers) override 
    {
        while (true) 
        {
            cout << "(C)reate user\n";
            cout << "(V)iew all users\n";
            cout << "(L)og out\n";
            cout << "Choice:";

            char choice;
            cin >> choice;

            if (choice == 'C') 
            {
                createNewUser(allUsers);
            } 
            else if (choice == 'V') 
            {
                cout << "\nRegistered System Users:\n";
                for (const auto* user : allUsers) 
                {
                    cout << "- " << user->getName() << "\n";
                }
            } 
            else if (choice == 'L') 
            {
                break;
            }
        }
    }
};

class App 
{
private:
    vector<Person*> users;

public:
    App() 
    {
        users.push_back(new Admin("Petka Skolova", "petaskolova@gmail.com", "123"));
    }

    Person* Login() 
    {
        string email, password;
        cout << "Log in\n";
        cout << "Email: ";
        cin >> email;
        cout << "Password: ";
        cin >> password;

        for (Person* u : users) 
        {
            if (u->checkData(email, password)) 
            {
                return u;
            }
        }
        cout << "Invalid login data\n";
        return nullptr;
    }

    void run() 
    {
        while (true) 
        {
            Person* loggedUser = Login();

            if (loggedUser != nullptr) 
            {
                loggedUser->userActions(users);
            }

            char exitChoice;
            cout << "Do you want to exit the app?(y/n): ";
            cin >> exitChoice;
            if (exitChoice == 'y') break;
        }
    }
};

int main() 
{
    App system;
    system.run();
    return 0;
}