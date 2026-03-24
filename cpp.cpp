#include <iostream>
#include <string>
#include <map>
#include <algorithm>
#include <memory>
#include <sstream>
#include <vector>
#include <cctype>

using namespace std;

// Класс исключения для некорректного IP-адреса
class InvalidIPException : public exception {
private:
    string message;
public:
    InvalidIPException(const string& ip) : message("Некорректный IP-адрес: " + ip) {}
    const char* what() const noexcept override {
        return message.c_str();
    }
};

// Функция для проверки корректности IP-адреса (упрощенная версия)
bool isValidIP(const string& ip) {
    if (ip.empty()) return false;
    
    vector<string> octets;
    stringstream ss(ip);
    string octet;
    
    // Разделяем строку по точкам
    while (getline(ss, octet, '.')) {
        octets.push_back(octet);
    }
    
    // IP-адрес должен состоять из 4 октетов
    if (octets.size() != 4) return false;
    
    for (const string& oct : octets) {
        // Проверяем, что октет не пустой и состоит только из цифр
        if (oct.empty()) return false;
        for (char c : oct) {
            if (!isdigit(c)) return false;
        }
        
        // Преобразуем в число и проверяем диапазон
        int num = stoi(oct);
        if (num < 0 || num > 255) return false;
    }
    
    return true;
}

// Базовый класс Device
class Device {
protected:
    string name;    // Имя устройства
    string ip;      // IP-адрес устройства
    bool status;    // Статус устройства (true - включен, false - выключен)

public:
    // Конструктор с проверкой IP
    Device(string n, string i, bool s) : name(n), ip(i), status(s) {
        if (!isValidIP(ip)) {
            throw InvalidIPException(ip);
        }
    }
    
    // Виртуальный деструктор
    virtual ~Device() {
        cout << "Устройство " << name << " (" << ip << ") удалено." << endl;
    }
    
    // Виртуальный метод для вывода информации
    virtual void info() const {
        cout << "===== Информация об устройстве =====" << endl;
        cout << "Имя: " << name << endl;
        cout << "IP-адрес: " << ip << endl;
        cout << "Статус: " << (status ? "Включен" : "Выключен") << endl;
    }
    
    // Перегрузка оператора == для сравнения по IP
    bool operator==(const Device& other) const {
        return this->ip == other.ip;
    }
    
    // Геттеры
    string getName() const { return name; }
    string getIp() const { return ip; }
    bool getStatus() const { return status; }
    
    // Сеттер для статуса
    void setStatus(bool s) { status = s; }
};

// Класс Server (наследуется от Device)
class Server : public Device {
private:
    int cpuCores;   // Количество ядер процессора
    int ram;        // Объем оперативной памяти (ГБ)

public:
    Server(string n, string i, bool s, int cores, int memory)
        : Device(n, i, s), cpuCores(cores), ram(memory) {
        if (cpuCores <= 0 || ram <= 0) {
            throw invalid_argument("Количество ядер и объем RAM должны быть положительными!");
        }
    }
    
    void info() const override {
        cout << "===== Информация о сервере =====" << endl;
        cout << "Имя: " << name << endl;
        cout << "IP-адрес: " << ip << endl;
        cout << "Статус: " << (status ? "Включен" : "Выключен") << endl;
        cout << "Ядра CPU: " << cpuCores << endl;
        cout << "RAM: " << ram << " ГБ" << endl;
    }
    
    int getCpuCores() const { return cpuCores; }
    int getRam() const { return ram; }
};

// Класс Client (наследуется от Device)
class Client : public Device {
private:
    string os;      // Операционная система

public:
    Client(string n, string i, bool s, string operatingSystem)
        : Device(n, i, s), os(operatingSystem) {
        if (os.empty()) {
            throw invalid_argument("Название ОС не может быть пустым!");
        }
    }
    
    void info() const override {
        cout << "===== Информация о клиенте =====" << endl;
        cout << "Имя: " << name << endl;
        cout << "IP-адрес: " << ip << endl;
        cout << "Статус: " << (status ? "Включен" : "Выключен") << endl;
        cout << "Операционная система: " << os << endl;
    }
    
    string getOs() const { return os; }
};

// Класс Router (наследуется от Device)
class Router : public Device {
private:
    int ports;      // Количество портов

public:
    Router(string n, string i, bool s, int p)
        : Device(n, i, s), ports(p) {
        if (ports <= 0) {
            throw invalid_argument("Количество портов должно быть положительным!");
        }
    }
    
    void info() const override {
        cout << "===== Информация о маршрутизаторе =====" << endl;
        cout << "Имя: " << name << endl;
        cout << "IP-адрес: " << ip << endl;
        cout << "Статус: " << (status ? "Включен" : "Выключен") << endl;
        cout << "Количество портов: " << ports << endl;
    }
    
    int getPorts() const { return ports; }
};

// Шаблонный класс Network
template <typename T>
class Network {
private:
    map<string, shared_ptr<T>> devices; // Контейнер map для хранения устройств (ключ - IP)

public:
    // Добавление устройства
    void addDevice(shared_ptr<T> device) {
        string ip = device->getIp();
        
        // Проверяем, существует ли уже устройство с таким IP
        if (devices.find(ip) != devices.end()) {
            cout << "Ошибка: Устройство с IP " << ip << " уже существует в сети!" << endl;
            return;
        }
        
        devices[ip] = device;
        cout << "Устройство \"" << device->getName() << "\" (IP: " << ip << ") добавлено в сеть." << endl;
    }
    
    // Удаление устройства по IP
    void removeDevice(const string& ip) {
        auto it = devices.find(ip);
        if (it != devices.end()) {
            cout << "Устройство \"" << it->second->getName() << "\" (IP: " << ip << ") удалено из сети." << endl;
            devices.erase(it);
        } else {
            cout << "Ошибка: Устройство с IP " << ip << " не найдено!" << endl;
        }
    }
    
    // Поиск устройства по IP (с использованием find)
    shared_ptr<T> findDevice(const string& ip) const {
        auto it = devices.find(ip);
        if (it != devices.end()) {
            return it->second;
        }
        return nullptr;
    }
    
    // Вывод информации обо всех устройствах в сети
    void showAllDevices() const {
        if (devices.empty()) {
            cout << "Сеть пуста." << endl;
            return;
        }
        
        cout << "\n===== СОСТОЯНИЕ СЕТИ =====" << endl;
        cout << "Всего устройств: " << devices.size() << endl;
        
        for (const auto& pair : devices) {
            cout << "\n--- Устройство ---" << endl;
            pair.second->info();
        }
    }
    
    // Поиск устройств по статусу (включены/выключены)
    void showDevicesByStatus(bool status) const {
        bool found = false;
        string statusStr = status ? "Включенные" : "Выключенные";
        cout << "\n===== " << statusStr << " устройства =====" << endl;
        
        for (const auto& pair : devices) {
            if (pair.second->getStatus() == status) {
                cout << " - " << pair.second->getName() << " (" << pair.first << ")" << endl;
                found = true;
            }
        }
        
        if (!found) {
            cout << "Нет " << (status ? "включенных" : "выключенных") << " устройств." << endl;
        }
    }
    
    // Получение количества устройств
    size_t getSize() const {
        return devices.size();
    }
};

// Главная функция
int main() {
    setlocale(LC_ALL, "ru"); // Для корректного вывода русского текста
    
    try {
        cout << "=== СОЗДАНИЕ УСТРОЙСТВ ===" << endl;
        
        // Создание устройств с проверкой IP
        auto server1 = make_shared<Server>("MainServer", "192.168.1.10", true, 16, 64);
        auto server2 = make_shared<Server>("BackupServer", "192.168.1.11", false, 8, 32);
        auto client1 = make_shared<Client>("Workstation1", "192.168.1.100", true, "Windows 11");
        auto client2 = make_shared<Client>("Workstation2", "192.168.1.101", true, "Ubuntu 22.04");
        auto router1 = make_shared<Router>("MainRouter", "192.168.1.1", true, 8);
        
        // Вывод информации о созданных устройствах
        server1->info();
        cout << endl;
        client1->info();
        cout << endl;
        router1->info();
        cout << endl;
        
        // Демонстрация проверки IP и обработки исключения
        cout << "\n=== ДЕМОНСТРАЦИЯ ОБРАБОТКИ ИСКЛЮЧЕНИЙ ===" << endl;
        try {
            auto invalidDevice = make_shared<Client>("BadDevice", "256.100.1.1", true, "TestOS");
        } catch (const InvalidIPException& e) {
            cerr << "Исключение: " << e.what() << endl;
        }
        
        try {
            auto invalidServer = make_shared<Server>("BadServer", "10.0.0.1", true, -4, 16);
        } catch (const invalid_argument& e) {
            cerr << "Исключение: " << e.what() << endl;
        }
        
        // Создание сети
        cout << "\n=== СОЗДАНИЕ СЕТИ ===" << endl;
        Network<Device> myNetwork;
        
        // Добавление устройств в сеть
        myNetwork.addDevice(server1);
        myNetwork.addDevice(server2);
        myNetwork.addDevice(client1);
        myNetwork.addDevice(client2);
        myNetwork.addDevice(router1);
        
        // Попытка добавить устройство с существующим IP
        auto duplicateClient = make_shared<Client>("Duplicate", "192.168.1.100", false, "macOS");
        myNetwork.addDevice(duplicateClient);
        
        // Вывод состояния сети
        myNetwork.showAllDevices();
        
        // Поиск устройства по IP с использованием find
        cout << "\n=== ПОИСК УСТРОЙСТВА ПО IP ===" << endl;
        string searchIp = "192.168.1.10";
        auto foundDevice = myNetwork.findDevice(searchIp);
        if (foundDevice) {
            cout << "Устройство с IP " << searchIp << " найдено:" << endl;
            foundDevice->info();
        } else {
            cout << "Устройство с IP " << searchIp << " не найдено." << endl;
        }
        
        // Поиск несуществующего устройства
        searchIp = "10.0.0.1";
        foundDevice = myNetwork.findDevice(searchIp);
        if (!foundDevice) {
            cout << "Устройство с IP " << searchIp << " не найдено." << endl;
        }
        
        // Демонстрация перегрузки оператора ==
        cout << "\n=== ДЕМОНСТРАЦИЯ ПЕРЕГРУЗКИ ОПЕРАТОРА == ===" << endl;
        Device device1("Temp1", "192.168.1.200", true);
        Device device2("Temp2", "192.168.1.200", false);
        Device device3("Temp3", "10.0.0.5", true);
        
        if (device1 == device2) {
            cout << "device1 и device2 имеют одинаковый IP: " << device1.getIp() << endl;
        } else {
            cout << "device1 и device2 имеют разные IP" << endl;
        }
        
        if (device1 == device3) {
            cout << "device1 и device3 имеют одинаковый IP" << endl;
        } else {
            cout << "device1 и device3 имеют разные IP" << endl;
        }
        
        // Поиск устройств по статусу
        myNetwork.showDevicesByStatus(true);
        myNetwork.showDevicesByStatus(false);
        
        // Удаление устройства из сети
        cout << "\n=== УДАЛЕНИЕ УСТРОЙСТВА ===" << endl;
        myNetwork.removeDevice("192.168.1.11");
        myNetwork.showAllDevices();
        
    } catch (const exception& ex) {
        cerr << "\n!!! НЕОБРАБОТАННОЕ ИСКЛЮЧЕНИЕ: " << ex.what() << " !!!" << endl;
    }
    
    cout << "\nПрограмма завершена." << endl;
    return 0;
}
