#include <iostream>
#include <string>
#include <windows.h>

using namespace std;
// ИНТЕРФЕЙСНЫЙ КЛАСС (все методы чисто виртуальные, объект создать нельзя)
class ICalendar {
public:
    virtual void Show() const = 0;          // вывод информации о календаре
    virtual double GetAnnualValue() const = 0; // полиморфный метод: для праздничного календаря – кол-во праздников,
                                               // для рабочего – кол-во рабочих дней
    virtual ~ICalendar() = 0;               // виртуальный деструктор обязателен
};

ICalendar::~ICalendar() {}
// ШАБЛОННЫЙ БАЗОВЫЙ КЛАСС (T – тип года: int, long long и т.д.)
//    Наследует интерфейс ICalendar, реализует общие поля и методы.
template <typename T>
class CalendarTemplate : public ICalendar {
private:
    static int totalCount;          // статическое поле – общее количество всех календарей

protected:
    string name;                    // название календаря (например, "Григорианский")
    T year;                         // год (тип параметризован)
    int daysInYear;                 // количество дней в году (обычный или високосный)

public:
    // Конструкторы
    CalendarTemplate() : name("Неизвестно"), year(0), daysInYear(365) {
        totalCount++;
    }

    CalendarTemplate(const string& n, T y, bool isLeap = false)
        : name(n), year(y), daysInYear(isLeap ? 366 : 365) {
        totalCount++;
    }

    virtual ~CalendarTemplate() {
        // деструктор – totalCount уменьшать не будем, чтобы счётчик показывал
        // количество когда-либо созданных объектов (как в примере)
    }

    // Полиморфный метод Show() – переопределён в потомках
    virtual void Show() const override {
        cout << "Календарь: " << name << endl;
        cout << "\tГод: " << year << endl;
        cout << "\tДней в году: " << daysInYear << endl;
    }

    // Полиморфный метод GetAnnualValue() – реализуется потомками
    virtual double GetAnnualValue() const override = 0; // чистый, чтобы потомки обязательно переопределили

    // Статический метод для вывода общего количества созданных календарей
    static void ShowTotalCount() {
        cout << "Всего создано календарей: " << totalCount << endl;
    }

    // Геттеры и сеттеры (инкапсуляция)
    string GetName() const { return name; }
    T GetYear() const { return year; }
    int GetDaysInYear() const { return daysInYear; }

    void SetName(const string& n) { name = n; }
    void SetYear(T y) { year = y; }
    void SetLeap(bool isLeap) { daysInYear = isLeap ? 366 : 365; }

    // Перегрузка оператора вывода (через виртуальный Show – полиморфизм)
    friend ostream& operator<<(ostream& os, const CalendarTemplate<T>& cal) {
        cal.Show();
        return os;
    }
};

// Инициализация статического поля
template <typename T>
int CalendarTemplate<T>::totalCount = 0;

//  ПЕРВЫЙ ПОТОМОК – ПРАЗДНИЧНЫЙ КАЛЕНДАРЬ (расширяет базовый класс)

template <typename T>
class HolidayCalendar : public CalendarTemplate<T> {
private:
    int holidayCount;               // количество праздничных дней в году
    static int holidayObjectCount;  // счётчик созданных объектов этого типа

public:
    // Конструкторы
    HolidayCalendar() : CalendarTemplate<T>(), holidayCount(0) {
        holidayObjectCount++;
    }

    HolidayCalendar(const string& name, T year, bool isLeap, int holidays)
        : CalendarTemplate<T>(name, year, isLeap), holidayCount(holidays) {
        holidayObjectCount++;
    }

    ~HolidayCalendar() {}

    // Переопределение полиморфного метода Show()
    virtual void Show() const override {
        CalendarTemplate<T>::Show(); // вызов метода родителя – повторное использование кода
        cout << "\tТип: Праздничный календарь" << endl;
        cout << "\tКоличество праздников: " << holidayCount << endl;
    }

    // Реализация второго полиморфного метода – возвращает количество праздников
    virtual double GetAnnualValue() const override {
        return static_cast<double>(holidayCount);
    }

    // Специфический метод для потомка (доступен только после dynamic_cast)
    int GetHolidayCount() const { return holidayCount; }

    // Статический метод для вывода количества созданных праздничных календарей
    static void ShowHolidayCount() {
        cout << "Всего создано праздничных календарей: " << holidayObjectCount << endl;
    }

    // Перегрузка оператора вывода
    friend ostream& operator<<(ostream& os, const HolidayCalendar<T>& cal) {
        cal.Show();
        return os;
    }
};

template <typename T>
int HolidayCalendar<T>::holidayObjectCount = 0;

// ВТОРОЙ ПОТОМОК – РАБОЧИЙ КАЛЕНДАРЬ (расширяет базовый класс)

template <typename T>
class WorkCalendar : public CalendarTemplate<T> {
private:
    int workDaysPerWeek;            // рабочих дней в неделе
    static int workObjectCount;     // счётчик созданных объектов этого типа

public:
    WorkCalendar() : CalendarTemplate<T>(), workDaysPerWeek(5) {
        workObjectCount++;
    }

    WorkCalendar(const string& name, T year, bool isLeap, int workDays)
        : CalendarTemplate<T>(name, year, isLeap), workDaysPerWeek(workDays) {
        workObjectCount++;
    }

    ~WorkCalendar() {}

    virtual void Show() const override {
        CalendarTemplate<T>::Show();
        cout << "\tТип: Рабочий календарь" << endl;
        cout << "\tРабочих дней в неделе: " << workDaysPerWeek << endl;
    }

    // Второй полиморфный метод – возвращает количество рабочих дней в году
    virtual double GetAnnualValue() const override {
        // приблизительный расчёт: 52 недели * workDaysPerWeek, минус праздники
        // для демонстрации достаточно простой формулы
        return 52.0 * workDaysPerWeek;
    }

    int GetWorkDaysPerWeek() const { return workDaysPerWeek; }

    static void ShowWorkCount() {
        cout << "Всего создано рабочих календарей: " << workObjectCount << endl;
    }

    friend ostream& operator<<(ostream& os, const WorkCalendar<T>& cal) {
        cal.Show();
        return os;
    }
};

template <typename T>
int WorkCalendar<T>::workObjectCount = 0;

// ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ ДЛЯ ВВОДА (с защитой от некорректного ввода)

string InputString(const string& prompt) {
    string value;
    cout << prompt;
    cin >> value;
    return value;
}

int InputInt(const string& prompt, bool positive = true) {
    int value;
    while (true) {
        cout << prompt;
        if (cin >> value) {
            if (!positive || value > 0) return value;
            cout << "Значение должно быть положительным!\n";
        } else {
            cout << "Ошибка ввода! Введите целое число.\n";
            cin.clear();
            cin.ignore(10000, '\n');
        }
    }
}

bool InputBool(const string& prompt) {
    string answer;
    cout << prompt << " (y/n): ";
    cin >> answer;
    return (answer == "y" || answer == "Y" || answer == "да" || answer == "Да");
}

// ГЛАВНАЯ ФУНКЦИЯ (демонстрация всех принципов ООП)

int main() {
    // Настройка консоли для русского языка (Windows)
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);

    int choice;
    do {
        cout << "1. Создать праздничный календарь (HolidayCalendar)\n";
        cout << "2. Создать рабочий календарь (WorkCalendar)\n";
        cout << "3. Показать количество созданных объектов\n";
        cout << "0. Выход\n";
        choice = InputInt("Ваш выбор: ", false); // false – разрешаем 0

        if (choice == 1) {
            // ---- Ввод данных для праздничного календаря ----
            string name = InputString("Введите название календаря: ");
            int year = InputInt("Введите год: ");
            bool isLeap = InputBool("Год високосный?");
            int holidays = InputInt("Введите количество праздничных дней: ");

            // ПОЗДНЕЕ СВЯЗЫВАНИЕ (динамический полиморфизм)
            // Указатель на базовый класс ICalendar, а объект создаётся как HolidayCalendar.
            // Какой именно метод Show() и GetAnnualValue() вызовется – определяется во время выполнения.
            ICalendar* cal = new HolidayCalendar<int>(name, year, isLeap, holidays);

            // Вывод через перегруженный оператор << (который внутри вызывает виртуальный Show)
            // Здесь сработает полиморфизм: будет вызван Show() класса HolidayCalendar.
            cout << "\n--- Создан объект (позднее связывание, указатель ICalendar*) ---\n";
            // Для вывода через << нужно привести к типу, так как оператор перегружен для конкретного шаблона.
            // Но у нас cal – это ICalendar*, а оператор << для ICalendar не перегружен.
            // Поэтому вызовем Show() напрямую, либо используем dynamic_cast для вывода через перегруженный оператор.
            // В примере со страной использовали *country, где country имел тип CountryTemplate<double>*,
            // а оператор << был перегружен для этого типа. У нас аналогично: cal имеет тип ICalendar*, но у ICalendar нет operator<<.
            // Поэтому для демонстрации полиморфизма вызовем Show().
            cal->Show();
            cout << "Полиморфное значение (GetAnnualValue): " << cal->GetAnnualValue() << " праздников\n";

            //РАННЕЕ СВЯЗЫВАНИЕ (через dynamic_cast)
            // Чтобы вызвать метод, специфичный только для HolidayCalendar (GetHolidayCount),
            // необходимо выполнить нисходящее приведение типа.
            HolidayCalendar<int>* hc = dynamic_cast<HolidayCalendar<int>*>(cal);
            if (hc) {
                cout << "Специфический метод GetHolidayCount(): " << hc->GetHolidayCount() << endl;
            } else {
                cout << "Ошибка приведения типа!\n";
            }

            delete cal; // освобождение памяти
        }
        else if (choice == 2) {
            // Ввод данных для рабочего календаря
            string name = InputString("Введите название календаря: ");
            int year = InputInt("Введите год: ");
            bool isLeap = InputBool("Год високосный?");
            int workDays = InputInt("Введите количество рабочих дней в неделе (обычно 5 или 6): ");

            // ОЗДНЕЕ СВЯЗЫВАНИЕ (аналогично)
            ICalendar* cal = new WorkCalendar<int>(name, year, isLeap, workDays);
            cal->Show();
            cout << "Полиморфное значение (GetAnnualValue): " << cal->GetAnnualValue() << " рабочих дней (приблизительно)\n";

            // РАННЕЕ СВЯЗЫВАНИЕ
            WorkCalendar<int>* wc = dynamic_cast<WorkCalendar<int>*>(cal);
            if (wc) {
                cout << "Специфический метод GetWorkDaysPerWeek(): " << wc->GetWorkDaysPerWeek() << endl;
            }
            delete cal;
        }
        else if (choice == 3) {
            // СТАТИЧЕСКИЕ МЕТОДЫ
            // Вызываются через имя класса, без создания объекта.
            CalendarTemplate<int>::ShowTotalCount();
            HolidayCalendar<int>::ShowHolidayCount();
            WorkCalendar<int>::ShowWorkCount();
        }

    } while (choice != 0);

    cout << "Программа завершена.\n";
    return 0;
}
