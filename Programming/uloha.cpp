#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

int lastId = 0;


vector<Person*> Users = {new Admin("Petka", "petaskolova@gmail.com", "123")};

Student* currentStudent = nullptr;
Instructor* currentInstructor = nullptr;
Admin* currentAdmin = nullptr;

char currentTask;


void LogIn();


class Person
{
    protected:
        string name;
        string email;
        string password;
        int id;

    

    public:

        virtual ~Person() = default;

        Person(string n, string e, string p)
        {
            id = lastId +1;
            name = n;
            email = e;
            password = p;
        }

        Person* trylogin(string e, string p)
        {
            if(e == email && p == password)
            { 
                return this;
            }
            return nullptr;
        }







};


class Student : public Person
{
    private:
    int drivesLeft = 17;

    
    public:

    Student(string n, string e, string p) : Person(n, e, p){}

    int getDrives(){ return drivesLeft;}
    void decreaseDrives(){ drivesLeft--;}


};

class Instructor : public Person
{
    public:

    Instructor(string n, string e, string p) : Person(n, e, p){}
};

class Admin : public Person
{
    public:

    Admin(string n, string e, string p) : Person(n, e, p){}

    void CreateUser()
    {
        string name;
        string email;
        string password;
        string type;

        cout << "Zadajte meno, email, heslo a typ uzivatela: \n";
        cin >> name >> email >> password >> type;

        if(type == "student") 
        {
            Student currentUser = Student(name, email, password);
        }
        else if(type == "instructor") 
        {
            
        }
        else if(type == "admin") 
        {
            
        }




    }
};

/*
private:
    int key;
    double value;

public:
    KeyValue(int k, double v);
    int GetKey();
    double GetValue();
};*/

int main()
{
    while(true)
    {
        LogIn();



        if (currentStudent != nullptr) 
        {
            
        }
        else if (currentInstructor != nullptr) 
        {
            
        }
        else if (currentAdmin != nullptr) 
        {
            while(true)
            {

                cout << "Create new account(C) or Log out(L)";
                cin >> currentTask;

                if(currentTask == 'C')
                {
                    char role;
                    string name;
                    string surename;
                    string email;
                    string password;
                    cout << "Input role(I, S, A), name, surename, email, password";
                    cin >> role >> name >> surename >> email >> password;

                    name = name + surename;

                    if(role == 'S')
                    {
                        Users.push_back(new Student(name, email, password));
                        Student* currentStudent = *Users[Users.size()-1];

                    }
                    else if(role == 'I')
                    {
                        Users.push_back(new Instructor(name, email, password));
                        Instructor* currentInstructor = *Users[Users.size()-1];

                    }
                    else if(role == 'A')
                    {
                        Users.push_back(new Admin(name, email, password));
                        Admin* currentAdmin = *Users[Users.size()-1];

                    }
                    else
                    {
                        cout << "Incorrect input";
                        break;
                    }
                    

                }
                else if(currentTask == 'L')
                {
                    break;
                    currentAdmin = nullptr;
                }
                else
                {
                    cout << "Incorret Input";
                }
            }

            
        }





    cout << "Type 'N' if you wanna leave app:";
    cin >> currentTask;
    if(currentTask == 'N')
    {
        return 0;
    }

     
    }
}



void LogIn()
{
    while(true)
    {
        string loginEmail;
        string loginPassword;
        
        cout << "enter email and password:\n";
        cin >> loginEmail >> loginPassword;

        for (int i = 0; i < Users.size(); i++)
        {
            Person* loggedUser = Users[i]->trylogin(loginEmail, loginPassword);
            
            if (loggedUser != nullptr)
            {
                
                if (Student* s = dynamic_cast<Student*>(loggedUser)) 
                {
                    currentStudent = s;
                }
                else if (Instructor* inst = dynamic_cast<Instructor*>(loggedUser)) 
                {
                    currentInstructor = inst;
                }
                else if (Admin* a = dynamic_cast<Admin*>(loggedUser)) 
                {
                    currentAdmin = a;
                }

                return;
            }

        }
        cout << "Incorrect login credentials or user doesn't exist";

    }

}