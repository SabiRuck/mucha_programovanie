
#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <algorithm>
#include <cstdlib>

using namespace std;

// ---------------------------------------------------------------------
// Pomocne funkcie pre vstup
// ---------------------------------------------------------------------
bool isNumber(const string& s)
{
    if (s.empty() || s.size() > 6) return false;   // dlhsie vstupy povazujeme za neplatne
    for (char c : s)
        if (c < '0' || c > '9') return false;
    return true;
}

int readInt(const string& prompt)
{
    while (true)
    {
        cout << prompt;
        int x;
        if (cin >> x) return x;
        if (cin.eof()) exit(0);
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "Zadaj cislo.\n";
    }
}

string readWord(const string& prompt)
{
    cout << prompt;
    string s;
    if (!(cin >> s)) exit(0);
    return s;
}

// ---------------------------------------------------------------------
// Cas jazdy (jedna jazda = 1 hodina)
// ---------------------------------------------------------------------
struct TimeSlot
{
    int week;   // 0 = tento tyzden, 1 = buduci...
    int day;    // 1 = pondelok ... 7 = nedela
    int hour;   // zaciatok jazdy, 6-20

    bool isValid() const { return week >= 0 && day >= 1 && day <= 7 && hour >= 6 && hour <= 20; }
    int toHours() const { return week * 168 + (day - 1) * 24 + hour; }
    bool operator==(const TimeSlot& o) const { return toHours() == o.toHours(); }

    string toString() const
    {
        static const char* names[] = {"Po", "Ut", "St", "Stv", "Pi", "So", "Ne"};
        return string(names[day - 1]) + " " + to_string(hour) + ":00 (tyzden " + to_string(week) + ")";
    }
};

// ---------------------------------------------------------------------
// Osoby
// ---------------------------------------------------------------------
class Autoskola;   // dopredna deklaracia - menu() ju potrebuje len ako referenciu

class Person
{
protected:
    static int lastId;
    int id;
    string name;
    string email;
    string password;

public:
    Person(const string& n, const string& e, const string& p)
        : id(++lastId), name(n), email(e), password(p) {}
    virtual ~Person() = default;

    bool checkLogin(const string& e, const string& p) const { return e == email && p == password; }
    const string& getName() const { return name; }
    const string& getEmail() const { return email; }
    int getId() const { return id; }

    virtual void menu(Autoskola& sys) = 0;   // kazda rola ma vlastne menu
};
int Person::lastId = 0;

class Student : public Person
{
    int drivesLeft = 17;

    void browseAndReserve(Autoskola& sys);
    void showMyLessons(Autoskola& sys);

public:
    Student(const string& n, const string& e, const string& p) : Person(n, e, p) {}

    int getDrives() const { return drivesLeft; }
    void useDrive() { drivesLeft--; }
    void returnDrive() { drivesLeft++; }

    void menu(Autoskola& sys) override;
};

class Instructor : public Person
{
public:
    Instructor(const string& n, const string& e, const string& p) : Person(n, e, p) {}
    void menu(Autoskola& sys) override;
};

class Admin : public Person
{
public:
    Admin(const string& n, const string& e, const string& p) : Person(n, e, p) {}
    void menu(Autoskola& sys) override;
};

// ---------------------------------------------------------------------
// Jazda a rezervacia
// ---------------------------------------------------------------------
class DrivingLesson
{
    int lessonID;
    TimeSlot slot;
    Instructor* instructor;
    Student* bookedBy = nullptr;

public:
    DrivingLesson(int id, TimeSlot s, Instructor* i) : lessonID(id), slot(s), instructor(i) {}

    bool isAvailable() const { return bookedBy == nullptr; }
    void book(Student* s) { bookedBy = s; }
    void release() { bookedBy = nullptr; }

    int getId() const { return lessonID; }
    const TimeSlot& getSlot() const { return slot; }
    Instructor* getInstructor() const { return instructor; }
    Student* getStudent() const { return bookedBy; }
};

class LessonReservation
{
    int reservationID;
    Student* student;
    DrivingLesson* lesson;

public:
    LessonReservation(int id, Student* s, DrivingLesson* l) : reservationID(id), student(s), lesson(l) {}

    int getId() const { return reservationID; }
    Student* getStudent() const { return student; }
    DrivingLesson* getLesson() const { return lesson; }
};

// ---------------------------------------------------------------------
// Autoskola = "System" + "Server": drzi data a pravidla
// Metody vracaju prazdny retazec pri uspechu, inak text chyby.
// ---------------------------------------------------------------------








class Autoskola
{
    vector<Person*> users;
    vector<DrivingLesson*> lessons;
    vector<LessonReservation*> reservations;
    int nextLessonId = 1;
    int nextReservationId = 1;
    TimeSlot now = {0, 1, 8};

public:
    Autoskola()
    {
        users.push_back(new Admin("Petka", "petaskolova@gmail.com", "123"));
    }

    ~Autoskola()
    {
        for (Person* u : users) delete u;
        for (DrivingLesson* l : lessons) delete l;
        for (LessonReservation* r : reservations) delete r;
    }

    Autoskola(const Autoskola&) = delete;
    Autoskola& operator=(const Autoskola&) = delete;

    Person* findUser(const string& email) const
    {
        for (Person* u : users)
            if (u->getEmail() == email) return u;
        return nullptr;
    }

    Person* login()
    {
        while (true)
        {
            cout << "\nPrihlasenie (email 'koniec' ukonci program)\n";
            string e, p;
            cout << "Email: ";
            if (!(cin >> e) || e == "koniec") return nullptr;
            cout << "Heslo: ";
            if (!(cin >> p)) return nullptr;

            for (Person* u : users)
                if (u->checkLogin(e, p)) return u;

            cout << "Nespravny email alebo heslo.\n";
        }
    }

    // Scenar 1: vytvorenie uctu
    string addUser(char role, const string& name, const string& email, const string& password)
    {
        if (name.empty() || email.empty() || password.empty()) return "Chyba povinny udaj.";
        if (email.find('@') == string::npos) return "Email musi obsahovat '@'.";
        if (findUser(email)) return "Tento email uz je pouzity.";

        switch (toupper(role))
        {
            case 'S': users.push_back(new Student(name, email, password)); break;
            case 'I': users.push_back(new Instructor(name, email, password)); break;
            case 'A': users.push_back(new Admin(name, email, password)); break;
            default: return "Neznama rola (pouzi S, I alebo A).";
        }
        return "";
    }


    
    // Scenar 2: pridanie terminu
    string addLesson(Instructor* ins, TimeSlot slot)
    {
        if (!slot.isValid()) return "Neplatny termin (tyzden >= 0, den 1-7, hodina 6-20).";
        if (slot.toHours() <= now.toHours()) return "Termin je v minulosti.";
        for (DrivingLesson* l : lessons)
            if (l->getInstructor() == ins && l->getSlot() == slot)
                return "V tomto case uz mas jazdu.";

        lessons.push_back(new DrivingLesson(nextLessonId++, slot, ins));
        return "";
    }

    vector<DrivingLesson*> lessonsInWeek(int week, bool onlyFree) const
    {
        vector<DrivingLesson*> result;
        for (DrivingLesson* l : lessons)
            if (l->getSlot().week == week && (!onlyFree || l->isAvailable()))
                result.push_back(l);
        sort(result.begin(), result.end(), [](DrivingLesson* a, DrivingLesson* b)
             { return a->getSlot().toHours() < b->getSlot().toHours(); });
        return result;
    }

    vector<DrivingLesson*> lessonsOf(Instructor* ins) const
    {
        vector<DrivingLesson*> result;
        for (DrivingLesson* l : lessons)
            if (l->getInstructor() == ins) result.push_back(l);
        sort(result.begin(), result.end(), [](DrivingLesson* a, DrivingLesson* b)
             { return a->getSlot().toHours() < b->getSlot().toHours(); });
        return result;
    }

    vector<LessonReservation*> reservationsOf(Student* s) const
    {
        vector<LessonReservation*> result;
        for (LessonReservation* r : reservations)
            if (r->getStudent() == s) result.push_back(r);
        sort(result.begin(), result.end(), [](LessonReservation* a, LessonReservation* b)
             { return a->getLesson()->getSlot().toHours() < b->getLesson()->getSlot().toHours(); });
        return result;
    }

    // Scenar 3: rezervacia (poradie krokov ako v sekvencnom diagrame)
    string reserve(Student* s, DrivingLesson* l)
    {
        if (s->getDrives() <= 0) return "Nemas ziadne zostavajuce jazdy.";
        if (l->getSlot().toHours() <= now.toHours()) return "Tato jazda uz zacala alebo prebehla.";
        for (LessonReservation* r : reservations)
            if (r->getStudent() == s && r->getLesson()->getSlot() == l->getSlot())
                return "V tomto case uz mas inu jazdu.";
        if (!l->isAvailable()) return "Termin medzitym zabral iny ziak.";

        l->book(s);
        s->useDrive();
        reservations.push_back(new LessonReservation(nextReservationId++, s, l));
        return "";
    }

    // Scenar 4: zrusenie rezervacie (max. 24 hodin vopred)
    string cancel(Student* s, LessonReservation* r)
    {
        if (r->getStudent() != s) return "Toto nie je tvoja rezervacia.";
        if (r->getLesson()->getSlot().toHours() - now.toHours() < 24)
            return "Zrusit sa da najneskor 24 hodin vopred.";
        removeReservation(r);
        return "";
    }

    // Situacia, ktoru system nezvladne predist: porucha vozidla tesne pred jazdou.
    // System moze len zrusit rezervaciu, vratit jazdu a informovat ziaka.
    string cancelDueToBreakdown(DrivingLesson* l)
    {
        for (LessonReservation* r : reservations)
            if (r->getLesson() == l)
            {
                removeReservation(r);
                return "";
            }
        return "Jazda nie je rezervovana.";
    }

private:
    void removeReservation(LessonReservation* r)
    {
        r->getLesson()->release();
        r->getStudent()->returnDrive();
        reservations.erase(find(reservations.begin(), reservations.end(), r));
        delete r;
    }
};


















// ---------------------------------------------------------------------
// Vypisy
// ---------------------------------------------------------------------
void printLessons(const vector<DrivingLesson*>& list)
{
    if (list.empty())
    {
        cout << "  (ziadne jazdy)\n";
        return;
    }
    for (size_t i = 0; i < list.size(); i++)
        cout << "  " << setw(2) << i + 1 << ") " << left << setw(24) << list[i]->getSlot().toString()
             << right << " instruktor: " << list[i]->getInstructor()->getName() << "\n";
}

void printReservations(const vector<LessonReservation*>& list)
{
    if (list.empty())
    {
        cout << "  (nemas ziadne rezervacie)\n";
        return;
    }
    for (size_t i = 0; i < list.size(); i++)
    {
        DrivingLesson* l = list[i]->getLesson();
        cout << "  " << setw(2) << i + 1 << ") " << left << setw(24) << l->getSlot().toString()
             << right << " instruktor: " << l->getInstructor()->getName() << "\n";
    }
}

// ---------------------------------------------------------------------
// Menu jednotlivych roli
// ---------------------------------------------------------------------
void Student::menu(Autoskola& sys)
{
    while (true)
    {
        cout << "\n--- Menu ziaka: " << name << " (zostava jazd: " << drivesLeft << ") ---\n"
             << "1) Volne terminy a rezervacia\n"
             << "2) Moje jazdy a zrusenie\n"
             << "L) Odhlasit\n> ";
        string c;
        if (!(cin >> c)) return;

        if (c == "1") browseAndReserve(sys);
        else if (c == "2") showMyLessons(sys);
        else if (c == "L" || c == "l") return;
        else cout << "Nerozumiem prikazu.\n";
    }
}

void Student::browseAndReserve(Autoskola& sys)
{
    int week = 0;
    while (true)
    {
        vector<DrivingLesson*> list = sys.lessonsInWeek(week, true);
        cout << "\nVolne jazdy - tyzden " << week << (week == 0 ? " (tento)" : "") << "\n";
        printLessons(list);
        cout << "Cislo = rezervovat, n = dalsi tyzden, p = predosly, q = spat\n> ";

        string c;
        if (!(cin >> c) || c == "q") return;

        if (c == "n") week++;
        else if (c == "p")
        {
            if (week > 0) week--;
            else cout << "Skorsie tyzdne nie su k dispozicii.\n";
        }
        else if (isNumber(c))
        {
            size_t n = stoul(c);
            if (n < 1 || n > list.size())
                cout << "Taka jazda v zozname nie je.\n";
            else
            {
                string err = sys.reserve(this, list[n - 1]);
                if (err.empty())
                    cout << "Rezervacia vytvorena: " << list[n - 1]->getSlot().toString()
                         << ", instruktor " << list[n - 1]->getInstructor()->getName() << ".\n";
                else
                    cout << "Rezervacia sa nepodarila: " << err << "\n";
            }
        }
        else cout << "Nerozumiem prikazu.\n";
    }
}

void Student::showMyLessons(Autoskola& sys)
{
    while (true)
    {
        vector<LessonReservation*> list = sys.reservationsOf(this);
        cout << "\nMoje jazdy:\n";
        printReservations(list);
        cout << "Cislo = zrusit, q = spat\n> ";

        string c;
        if (!(cin >> c) || c == "q") return;

        if (isNumber(c))
        {
            size_t n = stoul(c);
            if (n < 1 || n > list.size())
                cout << "Taka rezervacia v zozname nie je.\n";
            else
            {
                string err = sys.cancel(this, list[n - 1]);
                if (err.empty()) cout << "Rezervacia bola zrusena, termin je opat volny.\n";
                else cout << "Zrusenie sa nepodarilo: " << err << "\n";
            }
        }
        else cout << "Nerozumiem prikazu.\n";
    }
}

void Instructor::menu(Autoskola& sys)
{
    while (true)
    {
        cout << "\n--- Menu instruktora: " << name << " ---\n"
             << "1) Pridat termin jazdy\n"
             << "2) Moje jazdy\n"
             << "L) Odhlasit\n> ";
        string c;
        if (!(cin >> c)) return;

        if (c == "1")
        {
            TimeSlot slot;
            slot.week = readInt("Tyzden (0 = tento, 1 = buduci...): ");
            slot.day = readInt("Den (1 = pondelok ... 7 = nedela): ");
            slot.hour = readInt("Hodina zaciatku (6-20): ");

            string err = sys.addLesson(this, slot);
            if (err.empty()) cout << "Termin pridany: " << slot.toString() << ".\n";
            else cout << "Termin sa nepodarilo pridat: " << err << "\n";
        }
        else if (c == "2")
        {
            vector<DrivingLesson*> list = sys.lessonsOf(this);
            cout << "\nMoje jazdy:\n";
            if (list.empty()) cout << "  (ziadne jazdy)\n";
            for (DrivingLesson* l : list)
                cout << "  " << left << setw(24) << l->getSlot().toString() << right
                     << (l->isAvailable() ? " volna" : " rezervoval: " + l->getStudent()->getName()) << "\n";
        }
        else if (c == "L" || c == "l") return;
        else cout << "Nerozumiem prikazu.\n";
    }
}

void Admin::menu(Autoskola& sys)
{
    while (true)
    {
        cout << "\n--- Menu administratora: " << name << " ---\n"
             << "C) Vytvorit novy ucet\n"
             << "L) Odhlasit\n> ";
        string c;
        if (!(cin >> c)) return;

        if (c == "C" || c == "c")
        {
            string role = readWord("Rola (S = ziak, I = instruktor, A = admin): ");
            string first = readWord("Meno: ");
            string last = readWord("Priezvisko: ");
            string mail = readWord("Email: ");
            string pass = readWord("Heslo: ");

            string err = sys.addUser(role[0], first + " " + last, mail, pass);
            if (err.empty()) cout << "Ucet bol vytvoreny.\n";
            else cout << "Ucet sa nepodarilo vytvorit: " << err << "\n";
        }
        else if (c == "L" || c == "l") return;
        else cout << "Nerozumiem prikazu.\n";
    }
}

// ---------------------------------------------------------------------
// Aplikacia a demonstracia troch scenarov
// ---------------------------------------------------------------------
void runApp()
{
    Autoskola sys;
    while (true)
    {
        Person* user = sys.login();
        if (!user) break;
        user->menu(sys);   // polymorfizmus: spusti menu podla roly
    }
    cout << "Dovidenia.\n";
}

void runDemo()
{
    Autoskola sys;
    sys.addUser('I', "Jan Novak", "jan@auto.sk", "x");
    sys.addUser('S', "Anna Kovacova", "anna@mail.sk", "x");
    sys.addUser('S', "Boris Maly", "boris@mail.sk", "x");

    Instructor* jan = dynamic_cast<Instructor*>(sys.findUser("jan@auto.sk"));
    Student* anna = dynamic_cast<Student*>(sys.findUser("anna@mail.sk"));
    Student* boris = dynamic_cast<Student*>(sys.findUser("boris@mail.sk"));

    cout << "\n=== 1) IDEALNY SCENAR ===\n";
    sys.addLesson(jan, {0, 3, 10});
    sys.addLesson(jan, {0, 3, 12});
    vector<DrivingLesson*> list = sys.lessonsInWeek(0, true);
    cout << "Volne jazdy:\n";
    printLessons(list);
    string err = sys.reserve(anna, list[0]);
    cout << "Anna rezervuje jazdu 1: " << (err.empty() ? "OK, rezervacia vytvorena." : err) << "\n";
    cout << "Annin zostatok jazd: " << anna->getDrives() << "\n";

    cout << "\n=== 2) HRANICNY SCENAR (kolizia) ===\n";
    sys.addLesson(jan, {0, 4, 9});
    vector<DrivingLesson*> listAnna = sys.lessonsInWeek(0, true);
    vector<DrivingLesson*> listBoris = sys.lessonsInWeek(0, true);   // rovnaky zoznam
    err = sys.reserve(anna, listAnna[0]);
    cout << "Anna rezervuje jazdu 1: " << (err.empty() ? "OK" : err) << "\n";
    err = sys.reserve(boris, listBoris[0]);
    cout << "Boris rezervuje tu istu jazdu: " << (err.empty() ? "OK" : err) << "\n";
    cout << "Aktualizovany zoznam volnych terminov pre Borisa:\n";
    printLessons(sys.lessonsInWeek(0, true));

    cout << "\n=== 3) SITUACIA, KTORU SYSTEM NEZVLADNE ===\n";
    cout << "Porucha vozidla tesne pred jazdou - system tomu nevie zabranit.\n";
    DrivingLesson* broken = sys.reservationsOf(anna)[0]->getLesson();
    cout << "Anna ma jazd: " << anna->getDrives() << "\n";
    sys.cancelDueToBreakdown(broken);
    cout << "System mohol len zrusit rezervaciu a vratit jazdu. Anna ma jazd: "
         << anna->getDrives() << "\n";
    cout << "Informuj ziaka a dohodnite novy termin - to uz je mimo systemu.\n";
}

int main()
{
    cout << "1) Spustit aplikaciu\n2) Ukazka troch scenarov\n> ";
    string c;
    cin >> c;
    if (c == "2") runDemo();
    else runApp();
    return 0;
}