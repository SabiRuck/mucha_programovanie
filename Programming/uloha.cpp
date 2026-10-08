#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <ctime>
#include <map>

using namespace std;

class App;
class Instructor;
class Student;

//posledne id (nevedela osm ako inak spravit to aby sa neopakovali takze sa vzdy k tomu predchadzajucemu id pripocita +1)
int lastId = 0;
int lastLessonID = 0;

vector<string> dayNames = {
    "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"
};

//structy ktorymi si system pamata datumi a casy
struct TimeDateSlot {
    int day;
    int month;
    int year;
    int hour;
    int minute;
};

struct DateSlot {
    int day;
    int month;
    int year;
};

//funkcie na kontrolu spravnych inputov od uzivatela (dni, roky, atd)
bool isLeapYear(int year)
{
    return (year % 4 == 0 && year % 100 != 0) || year % 400 == 0;
}


int daysInMonth(int month, int year)
{
    if (month == 2)
    {
        if (isLeapYear(year))
            return 29;

        return 28;
    }

    if (month == 4 || month == 6 ||
        month == 9 || month == 11)
    {
        return 30;
    }

    return 31;
}


bool isValidDate(int day, int month, int year)
{
    if (year < 1 || year > 2100)
        return false;

    if (month < 1 || month > 12)
        return false;

    if (day < 1 || day > daysInMonth(month, year))
        return false;

    return true;
}


bool isValidEmail(const string& email)
{
    size_t at = email.find('@');

    if (at == string::npos || at == 0)
        return false;

    size_t dot = email.find('.', at + 1);

    if (dot == string::npos ||
        dot == at + 1 ||
        dot == email.length() - 1)
    {
        return false;
    }

    if (email.find('@', at + 1) != string::npos)
        return false;

    return true;
}

//overuje ci uzivatel zadal spravny cin
void clearInput()
{
    cin.clear();
    cin.ignore(1000, '\n');
}




class DrivingLesson
{
    int lessonID;
    TimeDateSlot slot;
    Instructor* instructor;
    Student* bookedBy = nullptr;

public:

    DrivingLesson(TimeDateSlot s, Instructor* i)
    {
        lessonID = lastLessonID + 1;
        lastLessonID = lessonID;
        slot = s;
        instructor = i;
    }

    bool isAvailable() { return bookedBy == nullptr; }

    //funkcie na rezervovanie lekcii
    void book(Student* s){ bookedBy = s;}
    void unbook(){bookedBy = nullptr;}

    TimeDateSlot& getTimeDateSlot(){return slot; }
    Instructor* getInstructor(){ return instructor; }
    Student* getStudent(){ return bookedBy; }
};

//parent class
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
        id = lastId + 1;
        lastId = lastId + 1;
        name = n;
        email = e;
        password = p;
    }

    virtual ~Person() = default;

    //kontrola ci zadane udaje sa zhodujus udajmi tohto uzivatela (pre login)
    bool checkData(const string& e, const string& p) const{ return (email == e && password == p); }

    string getName() const{ return name; }
    string getEmail() const{ return email;}

    virtual void userActions(App& sys) = 0;
};


class Student : public Person
{
private:
    int drivesLeft = 17; //kolko jazd ostava

public:

    Student(string n, string e, string p) : Person(n, e, p){}

    //predvytvorene funkcie - nizsie su urobene
    bool makeDriveReservation( App& sys, int week, const string& day, int lessonNumber);
    bool cancelDriveReservation( App& sys, int week, const string& day, int lessonNumber);
    void userActions(App& sys) override;
};


class Instructor : public Person
{
public:

    Instructor(string n, string e, string p): Person(n, e, p){}

    //predvytvorene funkcie - nizsie su urobene
    bool createDrivingLesson( App& sys, TimeDateSlot newSlot);
    void userActions(App& sys) override;
};


class Admin : public Person
{
public:

    Admin(string n, string e, string p) : Person(n, e, p){}

    //predvytvorene funkcie - nizsie su urobene
    Person* createUser( App& sys, char role, const string& name, const string& surname, const string& email, const string& password);
    void userActions(App& sys) override;
};


class App
{
private:
    vector<Person*> users;
    vector<DrivingLesson*> lessons;
    DateSlot today;

public:

    App()
    {
        //zakldany uzivatelia
        users.push_back(new Admin("Sabina Ruckova", "sabiruckova@gmail.com", "123"));
        users.push_back(new Instructor("Petka Skolova", "petaskolova@gmail.com", "123"));
        users.push_back(new Student("Peter Macus", "petermacus@gmail.com", "123"));

        //ziskanie dnesneho datumu
        time_t now_time = time(nullptr);
        tm* now = localtime(&now_time);

        today = {
            now->tm_mday,
            now->tm_mon + 1,
            now->tm_year + 1900
        };
    }

    //login funkcia - ak najde uzivatela vrati nanho pointer ak nie vrati nullptr
    Person* login(string email, string password)
        {
            for (Person* u : users)
            {
                if (u->checkData(email, password))
                {
                    return u;
                }
            }

            cout << "\nInvalid login data\n\n";
            return nullptr;
        }

    //funkcia pre CLI login
    Person* loginCLI()
    {
        string email, password;

        cout << "\nLog in\n";
        cout << "Email: ";
        cin >> email;

        cout << "Password: ";
        cin >> password;

        return login(email, password);
    }


    //prida usera do zoznamu
    void addUser(Person* user){ users.push_back(user); }
    //kontrola ci uzivatel nechce vytvorit ucet s uz pouzitim emailom
    bool emailExists(const string& email)
    {
        for (Person* user : users)
        {
            if (user->getEmail() == email)
                return true;
        }

        return false;
    }

    //vrati zoznam s dnami v tomto tyzdni(pre vypis jazd)
    vector<DateSlot> getWeek(int week)
    {
        vector<DateSlot> dates;

        tm date = {};
        date.tm_mday = today.day;
        date.tm_mon = today.month - 1;
        date.tm_year = today.year - 1900;

        mktime(&date);

        int daysFromMonday;

        if (date.tm_wday == 0)
            daysFromMonday = 6;
        else
            daysFromMonday = date.tm_wday - 1;

        date.tm_mday -= daysFromMonday;

        date.tm_mday += week * 7;

        mktime(&date);

        for (int i = 0; i < 7; i++)
        {
            DateSlot currentDate{
                date.tm_mday,
                date.tm_mon + 1,
                date.tm_year + 1900
            };

            dates.push_back(currentDate);

            date.tm_mday++;
            mktime(&date);
        }

        return dates;
    }

    //zisti kolko tyzdnov od dnesneho je dany den
    int findWeek(int day, int month, int year)
    {
        int week = 0;

        while (true)
        {
            vector<DateSlot> weekDates = getWeek(week);

            for (DateSlot date : weekDates)
            {
                if (date.day == day &&
                    date.month == month &&
                    date.year == year)
                {
                    return week;
                }
            }

            week++;
        }
    }

    //vrati vsetky lekcie v danom tyzdni podla dna v ktorom su
    map<string, vector<DrivingLesson*>> lessonsInWeek(int week)
    {
        vector<DateSlot> weekDates = getWeek(week);

        map<string, vector<DrivingLesson*>> result = {
            {"Mon", {}},
            {"Tue", {}},
            {"Wed", {}},
            {"Thu", {}},
            {"Fri", {}},
            {"Sat", {}},
            {"Sun", {}}
        };

        for (int i = 0; i < 7; i++)
        {
            for (DrivingLesson* lesson : lessons)
            {
                TimeDateSlot& slot =
                    lesson->getTimeDateSlot();

                if (slot.day == weekDates[i].day &&
                    slot.month == weekDates[i].month &&
                    slot.year == weekDates[i].year)
                {
                    result[dayNames[i]].push_back(lesson);
                }
            }
        }

        for (auto& day : result)
        {
            sort(
                day.second.begin(),
                day.second.end(),
                [](DrivingLesson* a, DrivingLesson* b)
                {
                    TimeDateSlot& timeA =
                        a->getTimeDateSlot();

                    TimeDateSlot& timeB =
                        b->getTimeDateSlot();

                    if (timeA.hour != timeB.hour)
                        return timeA.hour < timeB.hour;

                    return timeA.minute < timeB.minute;
                }
            );
        }

        return result;
    }

    vector<DrivingLesson*> getLessonsByDate(int day, int month, int year)
    {
        vector<DrivingLesson*> result;
        for (DrivingLesson* lesson : lessons)
        {
            TimeDateSlot& slot = lesson->getTimeDateSlot();
            if (slot.day == day && slot.month == month && slot.year == year)
            {
                result.push_back(lesson);
            }
        }
        return result;
    }

    //list lekcii pre studenta v 1 tyzdni - ak su rezervovane ich ani nezobrazi
    void printLessonsStud(int week)
    {
        vector<DateSlot> dates = getWeek(week);
        map<string, vector<DrivingLesson*>> weekLessons = lessonsInWeek(week);

        cout << "\n" << dates[0].day << "." << dates[0].month << "." << dates[0].year << " - " << dates[6].day << "." << dates[6].month << "." << dates[6].year << "\n";

        for (int i = 0; i < 7; i++)
        {
            string dayName = dayNames[i];
            cout << "\n" << dayName << ":\n";

            vector<DrivingLesson*>& dayLessons = weekLessons[dayName];

            bool hasAvailableLessons = false;
            for (DrivingLesson* lesson : dayLessons)
            {
                if (lesson->getStudent() == nullptr)
                {
                    hasAvailableLessons = true;
                    break;
                }
            }

            if (!hasAvailableLessons)
            {
                cout << "  No driving lessons\n";
                continue;
            }

            int counter = 1;
            for (int j = 0; j < dayLessons.size(); j++)
            {
                DrivingLesson* lesson = dayLessons[j];

                if (lesson->getStudent() != nullptr) continue;

                TimeDateSlot& slot = lesson->getTimeDateSlot();

                cout << "  " << counter++ << ". " << slot.day << "." << slot.month << "." << slot.year << " " << slot.hour << ":" << (slot.minute < 10 ? "0" : "") << slot.minute << " - " << lesson->getInstructor()->getName() << "\n";
            }
        }

        cout << "\n";
    }

    //list lekcii pre isntruktora - zobrazi len tie jeho
    void printLessonsInstr( int week, Instructor* instructor)
    {
        vector<DateSlot> dates = getWeek(week);

        map<string, vector<DrivingLesson*>> weekLessons =
            lessonsInWeek(week);


        cout << "\n";

        cout << dates[0].day << "."
            << dates[0].month << "."
            << dates[0].year
            << " - "
            << dates[6].day << "."
            << dates[6].month << "."
            << dates[6].year
            << "\n";

        cout << "Instructor: "
            << instructor->getName()
            << "\n";

        for (int i = 0; i < 7; i++)
        {
            string dayName = dayNames[i];

            cout << "\n"
                << dayName << ":\n";

            vector<DrivingLesson*>& dayLessons =
                weekLessons[dayName];

            int lessonNumber = 1;

            for (DrivingLesson* lesson : dayLessons)
            {
                if (lesson->getInstructor() != instructor)
                    continue;

                TimeDateSlot& slot =
                    lesson->getTimeDateSlot();

                cout << "  "
                    << lessonNumber << ". "
                    << slot.day << "."
                    << slot.month << "."
                    << slot.year << " "
                    << slot.hour << ":"
                    << (slot.minute < 10 ? "0" : "")
                    << slot.minute;

                if (!lesson->isAvailable())
                {
                    cout << " - Reserved by: "
                        << lesson->getStudent()->getName();
                }

                cout << "\n";

                lessonNumber++;
            }

            if (lessonNumber == 1)
            {
                cout << "  No driving lessons\n";
            }
        }

        cout << "\n";
    }


    DateSlot getToday(){return today;}

    //prida lekciu do listu
    void addLesson( TimeDateSlot slot, Instructor* instructor )
    {
        lessons.push_back(
            new DrivingLesson(slot, instructor)
        );
    }
};



//funkcie class pretoze som potrebovala definovanu App
Person* Admin::createUser( App& sys, char role, const string& name, const string& surname, const string& email, const string& password )
{
    //overuje ci su zadane data spravne(email atd)
    if (role != 'S' && role != 'I' && role != 'A')
    {
        cout << "\nInvalid role choice!\n\n";
        return nullptr;
    }

    if (!isValidEmail(email))
    {
        cout << "\nInvalid email.\n\n";
        return nullptr;
    }

    if (sys.emailExists(email))
    {
        cout << "\nThis email is already in use.\n\n";
        return nullptr;
    }

    string fullName = name + " " + surname;

    Person* newUser = nullptr;

    //zistuje aky ty usera ma byt vytvoreny a vytvori ho
    if (role == 'S')
    {
        newUser = new Student(fullName, email, password);
    }
    else if (role == 'I')
    {
        newUser = new Instructor(fullName, email, password);
    }
    else if (role == 'A')
    {
        newUser = new Admin(fullName, email, password);
    }

    sys.addUser(newUser);

    cout << "\nAccount created\n\n";

    return newUser;
}


bool Instructor::createDrivingLesson( App& sys, TimeDateSlot newSlot)
{
    int day = newSlot.day;
    int month = newSlot.month;
    int year = newSlot.year;
    int hour = newSlot.hour;
    int minute = newSlot.minute;

    //overuje ci je datum spravne zadany
    if (!isValidDate(day, month, year))
    {
        cout << "\nInvalid date.\n\n";
        return false;
    }

    //overuje ci je cas spravne zadany
    if (hour < 0 || hour > 23 ||  minute < 0 || minute > 59)
    {
        cout << "\nInvalid time.\n\n";
        return false;
    }

    int startMinutes = hour * 60 + minute;
    int endMinutes = startMinutes + 90;

    //overuje ci lepcia nie je dana az moc neskoro(neskoncila by az po polnoci)
    if (endMinutes > 24 * 60)
    {
        cout << "\nLesson is too late and would end after midnight.\n\n";
        return false;
    }

    DateSlot today = sys.getToday();

    time_t nowTime = time(nullptr);
    tm* now = localtime(&nowTime);

    //zistuje ci datum nie je v minulosti
    if (year < today.year || (year == today.year && month < today.month) || (year == today.year && month == today.month && day < today.day))
    {
        cout << "\nYou cannot add a lesson in the past.\n\n";
        return false;
    }

    //overuje ci cas nie je v minulosti ak je lekcia dnes
    if (year == today.year && month == today.month && day == today.day)
    {
        if (hour < now->tm_hour || (hour == now->tm_hour && minute < now->tm_min))
        {
            cout << "\nYou cannot add a lesson in the past.\n\n";
            return false;
        }
    }


    bool conflict = false;
    int newStart = hour * 60 + minute;
    int newEnd = newStart + 90;

    vector<DrivingLesson*> dayLessons = sys.getLessonsByDate(day, month, year);

    //overuje ci isntruktor nema uz lekciu v tom case
    for (DrivingLesson* lesson : dayLessons)
    {
        if (lesson->getInstructor() != this)
            continue;

        TimeDateSlot& existingSlot = lesson->getTimeDateSlot();

        int existingStart = existingSlot.hour * 60 + existingSlot.minute;
        int existingEnd = existingStart + 90;

        if (newStart < existingEnd && newEnd > existingStart)
        {
            conflict = true;
            break;
        }
    }
    if (conflict)
    {
        cout << "\nYou already have a lesson during this time.\n\n";
        return false;
    }

    sys.addLesson(newSlot, this);
    cout << "\nLesson added successfully.\n\n";
    return true;
}


bool Student::makeDriveReservation( App& sys, int week, const string& day, int lessonNumber)
{
    //kontrola ci je spravne zadane info k tej lekcii
    if (drivesLeft <= 0)
    {
        cout << "\nNo drives left!\n\n";
        return false;
    }

    map<string, vector<DrivingLesson*>> weekLessons = sys.lessonsInWeek(week);
    if (weekLessons.find(day) == weekLessons.end())
    {
        cout << "\nInvalid day.\n\n";
        return false;
    }

    vector<DrivingLesson*>& dayLessons = weekLessons[day];
    if (lessonNumber < 1 || lessonNumber > dayLessons.size())
    {
        cout << "\nInvalid lesson number.\n\n";
        return false;
    }

    DrivingLesson* selectedLesson = dayLessons[lessonNumber - 1];
    if (!selectedLesson->isAvailable())
    {
        cout << "\nThis lesson is already reserved.\n\n";
        return false;
    }


    selectedLesson->book(this);
    drivesLeft--;

    cout << "\nLesson successfully reserved.\n\n";

    return true;
}


bool Student::cancelDriveReservation( App& sys, int week, const string& day, int lessonNumber)
{
    //overenie ci su data spravne zadane
    map<string, vector<DrivingLesson*>> weekLessons =
        sys.lessonsInWeek(week);

    if (weekLessons.find(day) == weekLessons.end())
    {
        cout << "\nInvalid day.\n\n";
        return false;
    }

    vector<DrivingLesson*>& dayLessons =
        weekLessons[day];

    if (lessonNumber < 1 || lessonNumber > dayLessons.size())
    {
        cout << "\nInvalid lesson number.\n\n";
        return false;
    }

    DrivingLesson* selectedLesson =
        dayLessons[lessonNumber - 1];

    if (selectedLesson->isAvailable())
    {
        cout << "\nThis lesson is not reserved.\n\n";
        return false;
    }

    if (selectedLesson->getStudent() != this)
    {
        cout << "\nYou can only cancel your own lessons.\n\n";
        return false;
    }



    selectedLesson->unbook();
    drivesLeft++;

    cout << "\nLesson successfully cancelled.\n\n";

    return true;
}

//funkcie class ktore sa staraju o input uzivatela pri CLI
void Student::userActions(App& sys)
{
    while (true)
    {
        cout << "\n(V)iew drives left?\n";
        cout << "(R)eserve driving lesson\n";
        cout << "(C)ancel driving lesson\n";
        cout << "(L)og out\n";
        cout << "Choice: ";

        char choice;
        cin >> choice;
        
        //povie ziakovi kolko jazd mu ostava
        if (choice == 'V')
        {
            cout << "\nYou have " << drivesLeft << " drives left.\n\n";
        }


        else if (choice == 'R')
        {
            if (drivesLeft > 0)
            {
                int week = 0;

                while (true)
                {
                    sys.printLessonsStud(week);

                    cout << "\nShow (N)ext week\n";
                    cout << "Show (L)ast week\n";
                    cout << "(R)eserve lesson\n";
                    cout << "(G)o back\n";
                    cout << "Choice: ";

                    cin >> choice;

                    //posuvaju list lekcii pre studenta po tyzdnoch 
                    if (choice == 'N')
                    {
                        cout << "\n";
                        week++;
                    }
                    else if (choice == 'L')
                    {
                        if (week > 0)
                            week--;
                        else
                            cout << "\nYou are already viewing the current week.\n\n";

                    }
                    
                    //rezervuje lekciu
                    else if (choice == 'R')
                    {
                        string day;
                        int lessonNumber;

                        cout << "\nEnter day (Mon, Tue, Wed, Thu, Fri, Sat, Sun): ";
                        cin >> day;

                        cout << "Enter lesson number: ";

                        //kontrola ci uzivatel zadal spravne cislo
                        if (!(cin >> lessonNumber))
                        {
                            clearInput();

                            cout << "\nInvalid lesson number.\n\n";
                            continue;
                        }

                        makeDriveReservation( sys, week, day, lessonNumber);
                    }

                    //vrati uzivatela spat na minule okno
                    else if (choice == 'G')
                    {
                        cout << "\n";
                        break;
                    }
                }
            }
            else
            {
                cout << "\nNo drives left!\n\n";
            }
        }


        else if (choice == 'C')
        {
            int week = 0;

            while (true)
            {
                sys.printLessonsStud(week);

                cout << "\nShow (N)ext week\n";
                cout << "Show (L)ast week\n";
                cout << "(C)ancel lesson\n";
                cout << "(G)o back\n";
                cout << "Choice: ";

                cin >> choice;

                //posuvaju list lekcii pre studenta po tyzdnoch 
                if (choice == 'N')
                {
                    cout << "\n";
                    week++;
                }
                else if (choice == 'L')
                {

                    if (week > 0)
                        week--;
                    else
                        cout << "\nYou are already viewing the current week.\n\n";
                }
                
                
                else if (choice == 'C')
                {
                    string day;
                    int lessonNumber;

                    cout << "\nEnter day (Mon, Tue, Wed, Thu, Fri, Sat, Sun): ";
                    cin >> day;

                    cout << "Enter lesson number: ";

                    //kontrola ci uzivatel zadal spravne cislo
                    if (!(cin >> lessonNumber))
                    {
                        clearInput();

                        cout << "\nInvalid lesson number.\n\n";
                        continue;
                    }

                    cancelDriveReservation( sys, week, day, lessonNumber);
                }
                //vrati uzivatela spat na minule okno
                else if (choice == 'G')
                {
                    cout << "\n";
                    break;
                }
            }
        }

        //logout
        else if (choice == 'L')
        {
            cout << "\n";
            break;
        }
    }
}


void Instructor::userActions(App& sys)
{
    while (true)
    {
        cout << "\n(A)dd driving lesson\n";
        cout << "(V)iew reserved drives\n";
        cout << "(L)og out\n";
        cout << "Choice: ";

        char choice;
        cin >> choice;

        //pridava novu lekciu
        if (choice == 'A')
        {
            int day, month, year, hour, minute;

            cout << "\nEnter date (day month year): ";

            if (!(cin >> day >> month >> year))
            {
                clearInput();

                cout << "\nInvalid date.\n\n";
                continue;
            }

            cout << "Enter starting time (hour minute): ";

            if (!(cin >> hour >> minute))
            {
                clearInput();

                cout << "\nInvalid time.\n\n";
                continue;
            }

            TimeDateSlot newSlot{ day, month, year, hour, minute};

            createDrivingLesson( sys, newSlot);
        }

        //zobrazi lekcie pre instruktora aj ci a kym su rezervovane
        else if (choice == 'V')
        {
            int week = 0;

            while (true)
            {
                sys.printLessonsInstr( week, this );

                cout << "\nShow (N)ext week\n";
                cout << "Show (L)ast week\n";
                cout << "(G)o back\n";
                cout << "Choice: ";

                cin >> choice;

                if (choice == 'N')
                {
                    cout << "\n";
                    week++;
                }
                else if (choice == 'L')
                {
                    if (week > 0)
                    {
                        week--;
                    }
                    else
                    {
                        cout << "\nYou are already viewing the current week.\n\n";
                    }
                }
                else if (choice == 'G')
                {
                    cout << "\n";
                    break;
                }
            }
        }

        else if (choice == 'L')
        {
            cout << "\n";
            break;
        }
    }
}


void Admin::userActions(App& sys)
{
    while (true)
    {
        cout << "\n(C)reate user\n";
        cout << "(L)og out\n";
        cout << "Choice: ";

        char choice;
        cin >> choice;

        if (choice == 'C')
        {
            char role;
            string name, surname, email, password;

            cout << "\nAccount data:\n";

            cout << "Role (S-student,I-instructor,A-admin): ";
            cin >> role;

            cout << "Enter first name, surename, email, password:\n";
            cin >> name >> surname >> email >> password;

            createUser( sys, role, name, surname, email, password );
        }

        else if (choice == 'L')
        {
            cout << "\n";
            break;
        }
    }
}

//automaticky test kodu v ktorom sa nachadzaju aj neziadane scenare a ukazka ze su osetrene
void tryCode(App& testSystem)
{
    cout << "\nTRY CODE\n\n";

    //skusa loginnut admina ktoru uz je v zozname
    Person* loggedUser = testSystem.login("sabiruckova@gmail.com", "123");
    if (loggedUser != nullptr)
    {
        Admin* admin = dynamic_cast<Admin*>(dynamic_cast<Admin*>(loggedUser));

        if (admin != nullptr)//viem ze sa to podari ale tak je tu kontrola
        {
            cout << "Ideale scenare:\n";

            //pokusy o vytvorenie uctu adminom
            Person* instructorPerson = admin->createUser( testSystem, 'I', "Test", "Instructor", "test.instructor@gmail.com", "123");
            Person* studentPerson = admin->createUser( testSystem, 'S', "Test", "Student", "test.student@gmail.com", "123" );

            //vytvorime si variable pre tie ucty aby ste tu s nimi vedeli robit aj tu
            Instructor* instructor = dynamic_cast<Instructor*>(instructorPerson);
            Student* student = dynamic_cast<Student*>( studentPerson);

            //variable uz existujuceho uctu Studenta(login som skusala pri adminovi snad to teda zas netreba)
            Student* student2 = dynamic_cast<Student*>(testSystem.login("sabiruckova@gmail.com", "123"));

            //toto zisti datum buduceho pondelka => ten je v tyzdni "1" (dnesny tyzden je 0-kazdy dalsi +1 a tu treba az buduci pondelok (ak je dnes napr.streda tak by do pondelka v tomto tyzdni zapisovat neslo lebo je to minulost))
            vector<DateSlot> nextWeek = testSystem.getWeek(1);
            DateSlot lessonDate =nextWeek[0];

            //instruktor na ten datum vytvori 2 lekcie
            TimeDateSlot slot{ lessonDate.day, lessonDate.month, lessonDate.year, 10, 0};
            bool lessonCreated = instructor->createDrivingLesson(testSystem,slot);

            slot = { lessonDate.day, lessonDate.month, lessonDate.year, 12, 0};
            bool lessonCreated2 = instructor->createDrivingLesson(testSystem,slot);

            cout << "vypis lekcii z pohladu instruktora (kazdy vidi len svoje):";
            testSystem.printLessonsInstr( 1, instructor );//na mieste 1 je tyzden ktory to vypisuje(teraz potrebujeme len 1(buduci tyzden))

            if (lessonCreated && lessonCreated2)
            {
                cout << "Vypis lekcii ktore vidia studenti:";
                testSystem.printLessonsStud(1);

                //student sa prihlasi na tie 2 lekcie
                student->makeDriveReservation( testSystem, 1, "Mon", 1);//prva "1" je tyzden a druha "1" je poradie lekcie podla casu (mohla osm pouzit id ale to ma napadlo az moc neskoro)
                student->makeDriveReservation( testSystem, 1, "Mon", 2);
                
                cout << "Vypis lekcii ktore vidia studenti:";
                testSystem.printLessonsStud(1);

                //student zrusi rezervaciu na tu 2. lekciu
                student->cancelDriveReservation( testSystem, 1, "Mon", 2);

                cout << "Vypis lekcii ktore vidia studenti:";
                testSystem.printLessonsStud(1);

                cout << "vypis lekcii z pohladu instruktora (kazdy vidi len svoje):";
                testSystem.printLessonsInstr( 1, instructor );
            }

            cout << "Boundary scenare:\n";

            //vytvorenie uctu s mailom ktory bol pouzity
            admin->createUser( testSystem, 'S', "Another", "Student", "test.student@gmail.com", "123");

            //vytvorenie jazdy v neexistujucom nemoznom datume
            instructor->createDrivingLesson( testSystem, {31, 2, 2027, 10, 0});
            //vytvorenie lekcie v minulosti
            instructor->createDrivingLesson( testSystem, {10, 2, 2026, 10, 0});

            //rezervovanie neexistujucej jazdy
            student->makeDriveReservation( testSystem, 1, "Mon", 99);
            //student2 sa snazi zarezervovat uz obsadenu lekciu - ani by mu ju to neukazovalo takze je to vnimane ako neexistujuca(ale aj keby to nwm zaklikli obaja je tam aj kontrola dostupnosti lekcie)
            student2->makeDriveReservation( testSystem, 1, "Mon", 1);

            cout << "\n";
        }
    }   

    
}


int main()
{
    App system;

    while (true)
    {
        cout << "\nDRIVING SCHOOL\n";
        cout << "(L)ogin\n";
        cout << "(T)ry code\n";
        cout << "(E)xit\n";
        cout << "Choice: ";

        char choice;
        cin >> choice;

        if (choice == 'L')
        {
            cout << "\n";
            Person* loggedUser = system.loginCLI();
            if (loggedUser != nullptr)
            {
                loggedUser->userActions(system);
            }
        }
        else if (choice == 'T')
        {
            tryCode(system);
        }
        else if (choice == 'E')
        {
            cout << "\nGoodbye!\n";
            break;
        }
        else
        {
            cout << "\nInvalid choice.\n\n";
        }
    }

    return 0;
}

