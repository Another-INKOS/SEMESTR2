#include <iostream>
#include <string>
#include <windows.h>
#include <clocale>
#include <cctype>

using namespace std;

// ИНТЕРФЕЙСНЫЙ КЛАСС (все методы чисто виртуальные, нет полей)
// Иерархия начинается с этого класса.

class ICalendar {
public:
    // Чисто виртуальный метод для вывода информации о календаре
    virtual void Show() const = 0;
    // Чисто виртуальный метод, возвращающий годовую характеристику
    // (в праздничном календаре – число праздников, в рабочем – число рабочих дней)
    virtual double GetAnnualValue() const = 0;
    // Виртуальный деструктор обязателен для корректного удаления через указатель на интерфейс
    virtual ~ICalendar() = 0;
};
// Тело чисто виртуального деструктора всё равно нужно, чтобы он мог быть вызван при удалении
ICalendar::~ICalendar() {}

// ШАБЛОННЫЙ БАЗОВЫЙ КЛАСС (T – тип года)
// Реализует общие поля и методы для всех календарей.

template <typename T>
class CalendarTemplate : public ICalendar {
private:
    // СТАТИЧЕСКОЕ ПОЛЕ – общее для всех объектов любого типа календаря
    static int totalCount;   // считает общее количество когда-либо созданных календарей

protected:
    // ИНКАПСУЛЯЦИЯ: поля защищены, доступны наследникам, но не внешнему коду
    string name;   // название календаря
    T year;        // год (тип параметризован шаблоном)
    int daysInYear;// количество дней в году (только 365 или 366)

public:
    // Конструктор по умолчанию
    CalendarTemplate() : name("Неизвестно"), year(0), daysInYear(365) {
        totalCount++;   // увеличиваем общий счётчик при создании любого календаря
    }

    // Конструктор с параметрами
    CalendarTemplate(const string& n, T y, bool isLeap = false)
        : name(n), year(y), daysInYear(isLeap ? 366 : 365) {
        totalCount++;   // счётчик растёт
        // ОГРАНИЧЕНИЕ: дни в году не могут быть меньше 365 и больше 366
        if (daysInYear < 365) daysInYear = 365;
        if (daysInYear > 366) daysInYear = 366;
    }

    // Виртуальный деструктор – чтобы правильно уничтожать объекты-наследники
    virtual ~CalendarTemplate() {}

    // ПОЛИМОРФНЫЙ МЕТОД: виртуальный, переопределяется в потомках
    // Здесь – базовая реализация: вывод общих полей
    virtual void Show() const override {
        cout << "Календарь: " << name << endl;
        cout << "\tГод: " << year << endl;
        cout << "\tДней в году: " << daysInYear << endl;
    }

    // Второй полиморфный метод – чисто виртуальный, каждый потомок обязан его реализовать
    virtual double GetAnnualValue() const override = 0;

    // СТАТИЧЕСКИЙ МЕТОД: вызывается через имя класса без создания объекта
    static void ShowTotalCount() {
        cout << "Всего создано календарей (всех типов): " << totalCount << endl;
    }

    // Геттеры и сеттеры – обеспечивают ИНКАПСУЛЯЦИЮ (контролируемый доступ к полям)
    string GetName() const { return name; }
    T GetYear() const { return year; }
    int GetDaysInYear() const { return daysInYear; }

    void SetName(const string& n) { name = n; }
    void SetYear(T y) { year = y; }
    // Сеттер для дней в году снова применяет ограничение 365–366
    void SetDaysInYear(int days) {
        if (days < 365) days = 365;
        if (days > 366) days = 366;
        daysInYear = days;
    }

    // ПЕРЕГРУЗКА ОПЕРАТОРА << (дружественная функция)
    // Внутри вызывает виртуальный Show() – за счёт полиморфизма будет работать для любых потомков
    friend ostream& operator<<(ostream& os, const CalendarTemplate<T>& cal) {
        cal.Show();    // вызов виртуальной функции
        return os;
    }
};

// Инициализация статического поля (выделение памяти под счётчик)
template <typename T>
int CalendarTemplate<T>::totalCount = 0;

// ПЕРВЫЙ ПОТОМОК – ПРАЗДНИЧНЫЙ КАЛЕНДАРЬ (HolidayCalendar)
// Добавляет поле holidayCount – количество праздничных дней.

template <typename T>
class HolidayCalendar : public CalendarTemplate<T> {
private:
    int holidayCount;               // количество праздников в году
    static int holidayObjectCount;  // статический счётчик объектов этого типа

public:
    // Конструктор по умолчанию
    HolidayCalendar() : CalendarTemplate<T>(), holidayCount(0) {
        holidayObjectCount++;       // увеличиваем счётчик праздничных календарей
    }

    // Конструктор с параметрами
    HolidayCalendar(const string& name, T year, bool isLeap, int holidays)
        : CalendarTemplate<T>(name, year, isLeap), holidayCount(holidays) {
        holidayObjectCount++;       // счётчик праздничных календарей

        // ПАСХАЛКА (ТРЕТЬЕ СЕНТЯБРЯ)
        // Если в названии есть "сентябрь", "3 сентября" или holidays == 3,
        // выводим цитату из песни Шуфутинского.
        string lowerName = name;
        for (auto& c : lowerName) c = tolower(c);  // приводим к нижнему регистру
        if (lowerName.find("сентябрь") != string::npos ||
            lowerName.find("3 сентября") != string::npos ||
            holidays == 3) {
            cout << " Я календарь переверну — и снова третье сентября...\n";
            cout << "Шуфутинский одобряет этот календарь!\n";
        }
    }

    ~HolidayCalendar() {}

    // ПЕРЕОПРЕДЕЛЕНИЕ полиморфного метода Show()
    // Сначала вызываем родительскую версию, затем добавляем свои поля
    virtual void Show() const override {
        CalendarTemplate<T>::Show();   // вызов метода базового класса
        cout << "\tТип: Праздничный календарь" << endl;
        cout << "\tКоличество праздников: " << holidayCount << endl;
    }

    // Реализация второго полиморфного метода – возвращаем число праздников
    virtual double GetAnnualValue() const override {
        return static_cast<double>(holidayCount);
    }

    // Уникальный метод для этого потомка (доступен после приведения типа)
    int GetHolidayCount() const { return holidayCount; }

    // Статический метод для вывода количества праздничных календарей
    static void ShowHolidayCount() {
        cout << "Всего создано праздничных календарей: " << holidayObjectCount << endl;
    }

    // Дружественный оператор вывода (аналогично базовому)
    friend ostream& operator<<(ostream& os, const HolidayCalendar<T>& cal) {
        cal.Show();
        return os;
    }
};

// Инициализация статического счётчика праздничных календарей
template <typename T>
int HolidayCalendar<T>::holidayObjectCount = 0;

// ВТОРОЙ ПОТОМОК – РАБОЧИЙ КАЛЕНДАРЬ (WorkCalendar)
// Добавляет поле workDaysPerWeek – число рабочих дней в неделе.

template <typename T>
class WorkCalendar : public CalendarTemplate<T> {
private:
    int workDaysPerWeek;            // рабочих дней в неделе (обычно 5 или 6)
    static int workObjectCount;     // статический счётчик объектов этого типа

public:
    WorkCalendar() : CalendarTemplate<T>(), workDaysPerWeek(5) {
        workObjectCount++;
    }

    WorkCalendar(const string& name, T year, bool isLeap, int workDays)
        : CalendarTemplate<T>(name, year, isLeap), workDaysPerWeek(workDays) {
        workObjectCount++;
    }

    ~WorkCalendar() {}

    // Переопределение Show()
    virtual void Show() const override {
        CalendarTemplate<T>::Show();
        cout << "\tТип: Рабочий календарь" << endl;
        cout << "\tРабочих дней в неделе: " << workDaysPerWeek << endl;
    }

    // Реализация второго полиморфного метода – приблизительное число рабочих дней в году
    virtual double GetAnnualValue() const override {
        return 52.0 * workDaysPerWeek;   // 52 недели * рабочие дни в неделе
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

// ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ ДЛЯ ВВОДА (с защитой от некорректных данных)


// Ввод строки
string InputString(const string& prompt) {
    string value;
    cout << prompt;
    cin >> value;
    return value;
}

// Ввод целого числа (с проверкой на положительность и на ошибки ввода)
int InputInt(const string& prompt, bool positive = true) {
    int value;
    while (true) {                        // бесконечный цикл до корректного ввода
        cout << prompt;
        if (cin >> value) {               // если ввод успешен
            if (!positive || value > 0) return value;  // если положительное или не требуется положительность
            cout << "Значение должно быть положительным!\n";
        } else {                          // если ввод не число
            cout << "Ошибка! Введите целое число.\n";
            cin.clear();                  // сбрасываем флаг ошибки
            cin.ignore(10000, '\n');      // очищаем буфер ввода
        }
    }
}

// Ввод булева значения (y/n)
bool InputBool(const string& prompt) {
    string answer;
    cout << prompt << " (y/n): ";
    cin >> answer;
    return (answer == "y" || answer == "Y" || answer == "да" || answer == "Да");
}

int main() {
    setlocale(LC_ALL, "Russian");
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);

    int choice;
    do {
        // Вывод меню (пункты 1–4, 0 – выход)
        cout << "\nМЕНЮ КАЛЕНДАРЕЙ \n";
        cout << "1. Создать праздничный календарь\n";
        cout << "2. Создать рабочий календарь\n";
        cout << "3. Показать количество созданных объектов и 'лучшесть'\n";
        cout << "4. Перевернуть календарь (пасхалка)\n";
        cout << "0. Выход\n";
        choice = InputInt("Ваш выбор: ", false);   // false – разрешаем 0


        // Пункт 1: создание праздничного календаря

        if (choice == 1) {
            string name = InputString("Введите название календаря: ");
            int year = InputInt("Введите год: ");
            // Проверка на корректность года
            if (year <= 0) {
                cout << "Такого года существовать не может!\n";
                continue;   // возвращаемся в меню, не создавая объект
            }
            bool isLeap = InputBool("Год високосный?");
            int daysInYear = isLeap ? 366 : 365;   // определяем количество дней в году
            int holidays = InputInt("Введите количество праздничных дней: ");
            // Проверка: праздников не может быть больше дней в году
            if (holidays > daysInYear) {
                cout << "Праздничных дней не может быть больше дней в году!\n";
                continue;
            }

            // ПОЗДНЕЕ СВЯЗЫВАНИЕ: указатель на интерфейс ICalendar*,
            // объект – HolidayCalendar. Тип объекта определяется во время выполнения.
            ICalendar* cal = new HolidayCalendar<int>(name, year, isLeap, holidays);
            cout << "\n Создан праздничный календарь \n";
            cal->Show();   // вызов виртуального метода Show() – будет вызван метод HolidayCalendar::Show()
            cout << "Годовое значение (праздников): " << cal->GetAnnualValue() << endl;
            delete cal;    // освобождаем память
        }
        // Пункт 2: создание рабочего календаря
        else if (choice == 2) {
            string name = InputString("Введите название календаря: ");
            int year = InputInt("Введите год: ");
            if (year <= 0) {
                cout << "Такого года существовать не может!\n";
                continue;
            }
            bool isLeap = InputBool("Год високосный?");
            int workDays = InputInt("Введите количество рабочих дней в неделе: ");
            // Проверка: рабочие дни в неделе должны быть от 1 до 7
            if (workDays < 1 || workDays > 7) {
                cout << "Рабочих дней в неделе должно быть от 1 до 7!\n";
                continue;
            }

            ICalendar* cal = new WorkCalendar<int>(name, year, isLeap, workDays);
            cout << "\n--- Создан рабочий календарь ---\n";
            cal->Show();
            cout << "Годовое значение (рабочих дней, прибл.): " << cal->GetAnnualValue() << endl;
            delete cal;
        }
        // Пункт 3: статические счётчики и "лучшесть"
        else if (choice == 3) {
            cout << "\nСТАТИСТИКА\n";
            // Вызов статических методов через имена классов (без объектов)
            CalendarTemplate<int>::ShowTotalCount();
            HolidayCalendar<int>::ShowHolidayCount();
            WorkCalendar<int>::ShowWorkCount();

            cout << "\n Анализ: чем больше праздников, тем лучше! \n";
            cout << "Если у вас много праздничных календарей — отдыхайте чаще.\n";
            cout << "Рабочие календари — это хорошо, но праздники важнее для настроения.\n";
        }

        // Пункт 4: отдельная демонстрация пасхалки про третье сентября

        else if (choice == 4) {
            cout << "\n ТРЕТЬЕ СЕНТЯБРЯ \n";
            cout << "Я календарь переверну — и снова третье сентября.\n";
            cout << "День, когда Шуфутинский поёт в каждом сердце.\n";
            cout << "Создайте календарь с названием '3 сентября' или 'Сентябрь'\n";
            cout << "и пасхалка проявится автоматически!\n";
        }

    } while (choice != 0);   // повторять, пока не выберут 0

    cout << "Программа завершена. Спасибо за использование календарей!\n";
    return 0;
}
