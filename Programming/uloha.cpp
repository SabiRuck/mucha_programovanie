#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

int lastId = 0;


vector<Person*> Users = {new Admin("Petka", "petaskolova@gmail.com", "123")};




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
    string loginEmail;
    string loginPassword;
    
    cout << "enter email and password:\n";
    cin >> loginEmail >> loginPassword;

    for(int i=0; i<Users.size(); i++)
    {
        Person* loggedUser = Users[i]->trylogin(loginEmail, loginPassword);
        if (loggedUser != nullptr)
        {
            if(Student* currentUser = dynamic_cast<Student*>(loggedUser)) 
            {
            }
            else if(Instructor* currentUser = dynamic_cast<Instructor*>(loggedUser)) 
            {
                
            }
            else if(Admin* currentUser = dynamic_cast<Admin*>(loggedUser)) 
            {
                
            }

                break;
        }    
}








    return 0;
}