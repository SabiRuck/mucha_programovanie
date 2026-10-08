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

int lastId = 0;
int lastLessonID = 0;


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

class DrivingLesson
{
    int lessonID;
    TimeDateSlot slot;
    Instructor* instructor;
    Student* bookedBy = nullptr;

public:

    DrivingLesson(TimeDateSlot s, Instructor* i)
    {
        lessonID = lastLessonID+1;
        lastLessonID = lessonID;
        slot = s;
        instructor = i;
    }

    bool isAvailable() { return bookedBy==nullptr; }
    void book(Student* s) { bookedBy = s; }
    void unbook() { bookedBy = nullptr; }


    TimeDateSlot& getTimeDateSlot() { return slot; }
    Instructor* getInstructor() { return instructor; }
    Student* getStudent() { return bookedBy; }
};



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


    virtual void userActions(App& sys) = 0; 
};


class Student : public Person 
{
private:
    int drivesLeft = 17;

public:
    Student(string n, string e, string p) : Person(move(n), move(e), move(p)) {}

    void userActions(App& sys) override;


};


class Instructor : public Person 
{
public:
    Instructor(string n, string e, string p) : Person(move(n), move(e), move(p)) {}

    void userActions(App& sys) override;









};


class Admin : public Person 
{
private:

public:
    Admin(string n, string e, string p) : Person(move(n), move(e), move(p)) {}

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
            users.push_back(new Admin("Sabina Ruckova", "sabiruckova@gmail.com", "123"));
            users.push_back(new Instructor("Petka Skolova", "petaskolova@gmail.com", "123"));
            users.push_back(new Student("Peter Macus", "petermacus@gmail.com", "123"));

            time_t now_time = time(nullptr);
            tm* now = localtime(&now_time);

            today = {
                now->tm_mday,
                now->tm_mon + 1,
                now->tm_year + 1900,
            };

        }

    Person* login() 
    {
        string email, password;

        cout << "\n";
        cout << "Log in\n";
        cout << "Email: ";
        cin >> email;
        cout << "Password: ";
        cin >> password;

        for (Person* u : users) 
        {
            if (u->checkData(email, password)) 
            {
                cout << "\n";
                return u;
            }
        }

        cout << "\n";
        cout << "Invalid login data\n";
        cout << "\n";

        return nullptr;
    }


    void createNewUser() 
    {
        char role;
        string name, surname, email, password;

        cout << "\n";
        cout << "Account data:\n";
        cout << "Role (S-student,I-instructor,A-admin): ";
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
                cout << "\n";
                cout << "Invalid role choice!\n";
                cout << "\n";
                return;
        }

        users.push_back(newUser);

        cout << "\n";
        cout << "Account created\n";
        cout << "\n";
    }

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

        vector<string> dayNames = {
            "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"
        };

        for (int i = 0; i < 7; i++)
        {
            for (DrivingLesson* lesson : lessons)
            {
                TimeDateSlot& slot = lesson->getTimeDateSlot();

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
            sort(day.second.begin(), day.second.end(),
                [](DrivingLesson* a, DrivingLesson* b)
                {
                    TimeDateSlot& timeA = a->getTimeDateSlot();
                    TimeDateSlot& timeB = b->getTimeDateSlot();

                    if (timeA.hour != timeB.hour)
                        return timeA.hour < timeB.hour;

                    return timeA.minute < timeB.minute;
                });
        }

        return result;
    }


    void printLessonsStud(int week)
    {
        vector<DateSlot> dates = getWeek(week);
        map<string, vector<DrivingLesson*>> weekLessons = lessonsInWeek(week);

        vector<string> dayNames = {
            "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"
        };

        cout << "\n";
        cout << dates[0].day << "." << dates[0].month << "." << dates[0].year
            << " - "
            << dates[6].day << "." << dates[6].month << "." << dates[6].year
            << "\n";

        for (int i = 0; i < 7; i++)
        {
            string dayName = dayNames[i];

            cout << "\n" << dayName << ":\n";

            vector<DrivingLesson*>& dayLessons = weekLessons[dayName];

            if (dayLessons.empty())
            {
                cout << "  No driving lessons\n";
                continue;
            }

            for (int j = 0; j < dayLessons.size(); j++)
            {
                DrivingLesson* lesson = dayLessons[j];

                TimeDateSlot& slot = lesson->getTimeDateSlot();

                cout << "  " << j + 1 << ". "
                    << slot.day << "."
                    << slot.month << "."
                    << slot.year << " "
                    << slot.hour << ":"
                    << (slot.minute < 10 ? "0" : "")
                    << slot.minute
                    << " - "
                    << lesson->getInstructor()->getName()
                    << "\n";
            }
        }

        cout << "\n";
    }


    void printLessonsInstr(int week, Instructor* instructor)
    {
        vector<DateSlot> dates = getWeek(week);
        map<string, vector<DrivingLesson*>> weekLessons = lessonsInWeek(week);

        vector<string> dayNames = {
            "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"
        };

        cout << "\n";
        cout << dates[0].day << "." << dates[0].month << "." << dates[0].year
            << " - "
            << dates[6].day << "." << dates[6].month << "." << dates[6].year
            << "\n";

        cout << "Instructor: " << instructor->getName() << "\n";


        for (int i = 0; i < 7; i++)
        {
            string dayName = dayNames[i];

            cout << "\n" << dayName << ":\n";

            vector<DrivingLesson*>& dayLessons = weekLessons[dayName];

            int lessonNumber = 1;

            for (DrivingLesson* lesson : dayLessons)
            {
                if (lesson->getInstructor() != instructor)
                    continue;

                TimeDateSlot& slot = lesson->getTimeDateSlot();

                cout << "  " << lessonNumber << ". "
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

    DateSlot getToday()
    {
        return today;
    }

    void addLesson(TimeDateSlot slot, Instructor* instructor)
    {
        lessons.push_back(new DrivingLesson(slot, instructor));
    }


};

void Student::userActions(App& sys)
{
    while (true) 
    {
        cout << "\n";
        cout << "(V)iew drives left?\n";
        cout << "(R)eserve driving lesson\n";
        cout << "(C)ancel driving lesson\n";
        cout << "(L)og out\n";
        cout << "Choice: ";

        char choice;
        cin >> choice;

        if (choice == 'V') 
        {
            cout << "\n";
            cout << "You have " << drivesLeft << " drives left.\n";
            cout << "\n";
        }

        else if (choice == 'R')
        {
            if (drivesLeft > 0)
            {
                int week = 0;

                while (true)
                {
                    sys.printLessonsStud(week);

                    cout << "\n";
                    cout << "Show (N)ext week\n";
                    cout << "Show (L)ast week\n";
                    cout << "(R)eserve lesson\n";
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
                        cout << "\n";

                        if (week > 0)
                            week--;
                        else
                            cout << "You are already viewing the current week.\n";

                        cout << "\n";
                    }
                    else if (choice == 'R')
                    {
                        cout << "\n";

                        string day;
                        int lessonNumber;

                        cout << "Enter day (Mon, Tue, Wed, Thu, Fri, Sat, Sun): ";
                        cin >> day;

                        cout << "Enter lesson number: ";
                        cin >> lessonNumber;

                        map<string, vector<DrivingLesson*>> weekLessons =
                            sys.lessonsInWeek(week);

                        if (weekLessons.find(day) == weekLessons.end())
                        {
                            cout << "\n";
                            cout << "Invalid day.\n";
                            cout << "\n";
                            continue;
                        }

                        vector<DrivingLesson*>& dayLessons =
                            weekLessons[day];

                        if (lessonNumber < 1 ||
                            lessonNumber > dayLessons.size())
                        {
                            cout << "\n";
                            cout << "Invalid lesson number.\n";
                            cout << "\n";
                            continue;
                        }

                        DrivingLesson* selectedLesson =
                            dayLessons[lessonNumber - 1];

                        if (!selectedLesson->isAvailable())
                        {
                            cout << "\n";
                            cout << "This lesson is already reserved.\n";
                            cout << "\n";
                            continue;
                        }

                        selectedLesson->book(this);
                        drivesLeft--;

                        cout << "\n";
                        cout << "Lesson successfully reserved.\n";
                        cout << "\n";
                    }
                    else if (choice == 'G')
                    {
                        cout << "\n";
                        break;
                    }
                }
            }
            else
            {
                cout << "\n";
                cout << "No drives left!\n";
                cout << "\n";
            }
        }

        else if (choice == 'C')
        {
            int week = 0;

            while (true)
            {
                sys.printLessonsStud(week);

                cout << "\n";
                cout << "Show (N)ext week\n";
                cout << "Show (L)ast week\n";
                cout << "(C)ancel lesson\n";
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
                    cout << "\n";

                    if (week > 0)
                        week--;
                    else
                        cout << "You are already viewing the current week.\n";

                    cout << "\n";
                }
                else if (choice == 'C')
                {
                    cout << "\n";

                    string day;
                    int lessonNumber;

                    cout << "Enter day (Mon, Tue, Wed, Thu, Fri, Sat, Sun): ";
                    cin >> day;

                    cout << "Enter lesson number: ";
                    cin >> lessonNumber;

                    map<string, vector<DrivingLesson*>> weekLessons =
                        sys.lessonsInWeek(week);

                    if (weekLessons.find(day) == weekLessons.end())
                    {
                        cout << "\n";
                        cout << "Invalid day.\n";
                        cout << "\n";
                        continue;
                    }

                    vector<DrivingLesson*>& dayLessons =
                        weekLessons[day];

                    if (lessonNumber < 1 ||
                        lessonNumber > dayLessons.size())
                    {
                        cout << "\n";
                        cout << "Invalid lesson number.\n";
                        cout << "\n";
                        continue;
                    }

                    DrivingLesson* selectedLesson =
                        dayLessons[lessonNumber - 1];

                    if (selectedLesson->isAvailable())
                    {
                        cout << "\n";
                        cout << "This lesson is not reserved.\n";
                        cout << "\n";
                        continue;
                    }

                    if (selectedLesson->getStudent() != this)
                    {
                        cout << "\n";
                        cout << "You can only cancel your own lessons.\n";
                        cout << "\n";
                        continue;
                    }

                    selectedLesson->unbook();
                    drivesLeft++;

                    cout << "\n";
                    cout << "Lesson successfully cancelled.\n";
                    cout << "\n";
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


void Instructor::userActions(App& sys)
{
    while (true)
    {
        cout << "\n";
        cout << "(A)dd driving lesson\n";
        cout << "(V)iew reserved drives\n";
        cout << "(L)og out\n";
        cout << "Choice: ";

        char choice;
        cin >> choice;

        if (choice == 'A')
        {
            cout << "\n";

            int day, month, year;
            int hour, minute;

            cout << "Enter date (day month year): ";
            cin >> day >> month >> year;

            cout << "Enter starting time (hour minute): ";
            cin >> hour >> minute;

            DateSlot today = sys.getToday();

            // Cannot add a lesson in the past
            if (year < today.year ||
                (year == today.year && month < today.month) ||
                (year == today.year && month == today.month && day < today.day))
            {
                cout << "\n";
                cout << "You cannot add a lesson in the past.\n";
                cout << "\n";
                continue;
            }

            // Check time
            if (hour < 0 || hour > 23 ||
                minute < 0 || minute > 59)
            {
                cout << "\n";
                cout << "Invalid time.\n";
                cout << "\n";
                continue;
            }

            TimeDateSlot newSlot{
                day,
                month,
                year,
                hour,
                minute
            };

            // Find the week containing this date
            int week = 0;

            while (true)
            {
                vector<DateSlot> weekDates = sys.getWeek(week);

                bool found = false;

                for (DateSlot date : weekDates)
                {
                    if (date.day == day &&
                        date.month == month &&
                        date.year == year)
                    {
                        found = true;
                        break;
                    }
                }

                if (found)
                    break;

                week++;
            }

            map<string, vector<DrivingLesson*>> weekLessons =
                sys.lessonsInWeek(week);

            vector<string> dayNames = {
                "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"
            };

            vector<DateSlot> weekDates = sys.getWeek(week);

            string selectedDay;

            for (int i = 0; i < 7; i++)
            {
                if (weekDates[i].day == day &&
                    weekDates[i].month == month &&
                    weekDates[i].year == year)
                {
                    selectedDay = dayNames[i];
                    break;
                }
            }

            bool conflict = false;

            int newStart = hour * 60 + minute;
            int newEnd = newStart + 90;

            for (DrivingLesson* lesson : weekLessons[selectedDay])
            {
                if (lesson->getInstructor() != this)
                    continue;

                TimeDateSlot& existingSlot =
                    lesson->getTimeDateSlot();

                int existingStart =
                    existingSlot.hour * 60 + existingSlot.minute;

                int existingEnd = existingStart + 90;

                if (newStart < existingEnd &&
                    newEnd > existingStart)
                {
                    conflict = true;
                    break;
                }
            }

            if (conflict)
            {
                cout << "\n";
                cout << "You already have a lesson during this time.\n";
                cout << "\n";
                continue;
            }

            sys.addLesson(newSlot, this);

            cout << "\n";
            cout << "Lesson added successfully.\n";
            cout << "\n";
        }
        else if (choice == 'V')
        {
            int week = 0;

            while (true)
            {
                sys.printLessonsInstr(week, this);

                cout << "\n";
                cout << "Show (N)ext week\n";
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
                    cout << "\n";

                    if (week > 0)
                    {
                        week--;
                    }
                    else
                    {
                        cout << "You are already viewing the current week.\n";
                    }

                    cout << "\n";
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
        cout << "\n";
        cout << "(C)reate user\n";
        cout << "(L)og out\n";
        cout << "Choice:";

        char choice;
        cin >> choice;

        if (choice == 'C') 
        {
            cout << "\n";
            sys.createNewUser();
        } 

        else if (choice == 'L') 
        {
            cout << "\n";
            break;
        }
    }
}


int main()
{
    App system;

    while (true)
    {
        cout << "\n";
        cout << "DRIVING SCHOOL\n";
        cout << "(L)ogin\n";
        cout << "(E)xit\n";
        cout << "Choice: ";

        char choice;
        cin >> choice;

        if (choice == 'L')
        {
            cout << "\n";

            Person* loggedUser = system.login();

            if (loggedUser != nullptr)
            {
                loggedUser->userActions(system);
            }

            cout << "\n";
        }
        else if (choice == 'E')
        {
            cout << "\n";
            cout << "Goodbye!\n";
            break;
        }
        else
        {
            cout << "\n";
            cout << "Invalid choice.\n";
            cout << "\n";
        }
    }

    return 0;
}