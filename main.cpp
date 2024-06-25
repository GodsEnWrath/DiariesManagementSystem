#include <iostream>
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <ctime>
#include <vector>
#include <algorithm>

using namespace std;

class Database {
private:
    sql::Connection* con;
    sql::mysql::MySQL_Driver* driver;
    sql::Statement* stmt;
    sql::PreparedStatement* prep_stmt;

public:
    Database() : con(nullptr), driver(nullptr), stmt(nullptr), prep_stmt(nullptr) {}

    ~Database() {
        if (stmt) delete stmt;
        if (con) delete con;
        if (prep_stmt) delete prep_stmt;
    }

    void createDB() {
        try {
            driver = sql::mysql::get_mysql_driver_instance();
            con = driver->connect("tcp://localhost:3306", "root", "your_password");

            con->setSchema("diaries");
            stmt = con->createStatement();

            // SQL query to create a table user_types
            string createTableSQL1 =
                "CREATE TABLE IF NOT EXISTS user_types ("
                "id INT NOT NULL AUTO_INCREMENT PRIMARY KEY,"
                "user_type INT NOT NULL)";

            stmt->execute(createTableSQL1);

            // SQL query to create a table user_base
            string createTableSQL2 =
                "CREATE TABLE IF NOT EXISTS user_base ("
                "id INT NOT NULL AUTO_INCREMENT PRIMARY KEY,"
                "user_type INT unsigned NOT NULL,"
                "username VARCHAR(16) NOT NULL,"
                "password VARCHAR(16) NOT NULL,"
                "name VARCHAR(150) NOT NULL,"
                "date_updated DATE NOT NULL,"
                "date_creation DATE NOT NULL"
                ")";

            stmt->execute(createTableSQL2);

            // SQL query to create a table user_diaries
            string createTableSQL3 =
                "CREATE TABLE IF NOT EXISTS user_diaries ("
                "id INT NOT NULL AUTO_INCREMENT PRIMARY KEY,"
                "user_id INT unsigned NOT NULL,"
                "diaries_entry TEXT NOT NULL,"
                "date_updated DATE NOT NULL,"
                "date_creation DATE NOT NULL"
                ")";

            stmt->execute(createTableSQL3);

            cout << "Initialize DB Completed" << endl;

        }
        catch (sql::SQLException& e) {
            cerr << "SQL Error: " << e.what() << endl;
        }
    }

    void registerUser(int userType, const string& username, const string& password, const string& name) {
        try {
            prep_stmt = con->prepareStatement("INSERT INTO user_base(user_type, username, password, name, date_creation, date_updated) VALUES(?,?,?,?,NOW(),NOW())");
            prep_stmt->setInt(1, userType);
            prep_stmt->setString(2, username);
            prep_stmt->setString(3, password);
            prep_stmt->setString(4, name);
            prep_stmt->execute();
            cout << "User registered successfully." << endl;
        }
        catch (sql::SQLException& e) {
            cerr << "SQL Error: " << e.what() << endl;
        }
    }

    bool loginUser(const string& username, const string& password, int& userType, int& userId) {
        try {
            prep_stmt = con->prepareStatement("SELECT id, user_type FROM user_base WHERE username=? AND password=?");
            prep_stmt->setString(1, username);
            prep_stmt->setString(2, password);
            sql::ResultSet* res = prep_stmt->executeQuery();
            if (res->next()) {
                userId = res->getInt("id");
                userType = res->getInt("user_type");
                delete res;
                return true;
            }
            delete res;
        }
        catch (sql::SQLException& e) {
            cerr << "SQL Error: " << e.what() << endl;
        }
        return false;
    }

    void showAllUsers() {
        try {
            prep_stmt = con->prepareStatement("SELECT * FROM user_base");
            sql::ResultSet* res = prep_stmt->executeQuery();
            while (res->next()) {
                cout << "ID: " << res->getInt("id")
                    << ", Username: " << res->getString("username")
                    << ", Name: " << res->getString("name")
                    << ", Date Created: " << res->getString("date_creation")
                    << ", Date Updated: " << res->getString("date_updated") << endl;
            }
            delete res;
        }
        catch (sql::SQLException& e) {
            cerr << "SQL Error: " << e.what() << endl;
        }
    }

    void updateUser(int userId, const string& name) {
        try {
            prep_stmt = con->prepareStatement("UPDATE user_base SET name=?, date_updated=NOW() WHERE id=?");
            prep_stmt->setString(1, name);
            prep_stmt->setInt(2, userId);
            prep_stmt->execute();
            cout << "User information updated successfully." << endl;
        }
        catch (sql::SQLException& e) {
            cerr << "SQL Error: " << e.what() << endl;
        }
    }

    void updateUserPassword(int userId, const string& password) {
        try {
            prep_stmt = con->prepareStatement("UPDATE user_base SET password=?, date_updated=NOW() WHERE id=?");
            prep_stmt->setString(1, password);
            prep_stmt->setInt(2, userId);
            prep_stmt->execute();
            cout << "User password updated successfully." << endl;
        }
        catch (sql::SQLException& e) {
            cerr << "SQL Error: " << e.what() << endl;
        }
    }

    void deleteUser(int userId) {
        try {
            prep_stmt = con->prepareStatement("DELETE FROM user_base WHERE id=?");
            prep_stmt->setInt(1, userId);
            prep_stmt->execute();
            cout << "User deleted successfully." << endl;
        }
        catch (sql::SQLException& e) {
            cerr << "SQL Error: " << e.what() << endl;
        }
    }

    void createDiaryEntry(int userId, const string& diaryEntry) {
        try {
            prep_stmt = con->prepareStatement("INSERT INTO user_diaries(user_id, diaries_entry, date_creation, date_updated) VALUES(?,?,NOW(),NOW())");
            prep_stmt->setInt(1, userId);
            prep_stmt->setString(2, diaryEntry);
            prep_stmt->execute();
            cout << "Diary entry created successfully." << endl;
        }
        catch (sql::SQLException& e) {
            cerr << "SQL Error: " << e.what() << endl;
        }
    }

    void showDiaryEntries(int userId) {
        try {
            prep_stmt = con->prepareStatement("SELECT id, diaries_entry, date_creation FROM user_diaries WHERE user_id=? ORDER BY date_creation DESC");
            prep_stmt->setInt(1, userId);
            sql::ResultSet* res = prep_stmt->executeQuery();
            vector<pair<int, string>> entries;
            while (res->next()) {
                entries.emplace_back(res->getInt("id"), res->getString("diaries_entry") + " (Date: " + res->getString("date_creation") + ")");
            }
            delete res;

            sort(entries.begin(), entries.end(), [](const pair<int, string>& a, const pair<int, string>& b) {
                return a.second > b.second; // Descending order
                });

            cout << "Diary Entries:" << endl;
            for (const auto& entry : entries) {
                cout << "ID: " << entry.first << ", Entry: " << entry.second << endl;
            }
        }
        catch (sql::SQLException& e) {
            cerr << "SQL Error: " << e.what() << endl;
        }
    }

    void showAllDiaryEntries() {
        try {
            prep_stmt = con->prepareStatement("SELECT id, user_id, diaries_entry, date_creation FROM user_diaries ORDER BY date_creation DESC");
            sql::ResultSet* res = prep_stmt->executeQuery();
            while (res->next()) {
                cout << "Diary ID: " << res->getInt("id")
                    << ", User ID: " << res->getInt("user_id")
                    << ", Entry: " << res->getString("diaries_entry")
                    << ", Date: " << res->getString("date_creation") << endl;
            }
            delete res;
        }
        catch (sql::SQLException& e) {
            cerr << "SQL Error: " << e.what() << endl;
        }
    }

    void updateDiaryEntry(int diaryId, const string& diaryEntry) {
        try {
            prep_stmt = con->prepareStatement("UPDATE user_diaries SET diaries_entry=?, date_updated=NOW() WHERE id=?");
            prep_stmt->setString(1, diaryEntry);
            prep_stmt->setInt(2, diaryId);
            prep_stmt->execute();
            cout << "Diary entry updated successfully." << endl;
        }
        catch (sql::SQLException& e) {
            cerr << "SQL Error: " << e.what() << endl;
        }
    }

    void deleteDiaryEntry(int diaryId) {
        try {
            prep_stmt = con->prepareStatement("DELETE FROM user_diaries WHERE id=?");
            prep_stmt->setInt(1, diaryId);
            prep_stmt->execute();
            cout << "Diary entry deleted successfully." << endl;
        }
        catch (sql::SQLException& e) {
            cerr << "SQL Error: " << e.what() << endl;
        }
    }
};

void adminMenu(Database& database) {
    int option;
    do {
        cout << "Welcome administrator" << endl;
        cout << "1. Show all active members" << endl;
        cout << "2. Update existing user information" << endl;
        cout << "3. Update user password" << endl;
        cout << "4. Delete an account" << endl;
        cout << "5. View all diary entries" << endl;
        cout << "6. Edit a diary entry" << endl;
        cout << "7. Delete a diary entry" << endl;
        cout << "8. Logout" << endl;
        cout << "Enter your choice: ";
        cin >> option;

        int userId, diaryId;
        string name, password, diaryEntry;

        switch (option) {
        case 1:
            database.showAllUsers();
            break;
        case 2:
            cout << "Enter user ID to update: ";
            cin >> userId;
            cout << "Enter new name: ";
            cin >> name;
            database.updateUser(userId, name);
            break;
        case 3:
            cout << "Enter user ID to update password: ";
            cin >> userId;
            cout << "Enter new password: ";
            cin >> password;
            database.updateUserPassword(userId, password);
            break;
        case 4:
            cout << "Enter user ID to delete: ";
            cin >> userId;
            database.deleteUser(userId);
            break;
        case 5:
            database.showAllDiaryEntries();
            break;
        case 6:
            cout << "Enter diary ID to update: ";
            cin >> diaryId;
            cout << "Enter new diary entry: ";
            cin.ignore();
            getline(cin, diaryEntry);
            database.updateDiaryEntry(diaryId, diaryEntry);
            break;
        case 7:
            cout << "Enter diary ID to delete: ";
            cin >> diaryId;
            database.deleteDiaryEntry(diaryId);
            break;
        case 8:
            cout << "Logging out..." << endl;
            break;
        default:
            cout << "Invalid option!" << endl;
            break;
        }
    } while (option != 8);
}

void userMenu(Database& database, int userId) {
    int option;
    do {
        cout << "1. Create Diary Entry" << endl;
        cout << "2. View and Manage Diary Entries" << endl;
        cout << "3. Edit Diary Entry" << endl;
        cout << "4. Logout" << endl;
        cout << "Enter your choice: ";
        cin >> option;

        string diaryEntry;
        int diaryId;

        switch (option) {
        case 1:
            cout << "Enter your diary entry: ";
            cin.ignore(); // Clear newline left in the buffer
            getline(cin, diaryEntry);
            database.createDiaryEntry(userId, diaryEntry);
            break;
        case 2:
            database.showDiaryEntries(userId);
            cout << "Enter the ID of the entry you want to delete (0 to cancel): ";
            cin >> diaryId;
            if (diaryId != 0) {
                database.deleteDiaryEntry(diaryId);
            }
            break;
        case 3:
            database.showDiaryEntries(userId);
            cout << "Enter the ID of the entry you want to edit: ";
            cin >> diaryId;
            cout << "Enter the new diary entry: ";
            cin.ignore();
            getline(cin, diaryEntry);
            database.updateDiaryEntry(diaryId, diaryEntry);
            break;
        case 4:
            cout << "Logging out..." << endl;
            break;
        default:
            cout << "Invalid option!" << endl;
            break;
        }
    } while (option != 4);
}

void welcomePage() {
    cout << "Welcome to Diaries Management Systems By: Muhammad Yazid Alfarisi" << endl;
    cout << "Please choose register if you are new" << endl;
    cout << "Choose Login if you are an existing user" << endl;
    cout << "1. Login" << endl;
    cout << "2. Register" << endl; 
    cout << "3. Quit" << endl;
    cout << "Enter your choice: ";
}

void registerPage(Database& database) {
    int userType;
    string username, password, name;
    cout << "Are you an admin? Enter 1 if Yes and 2 for No: ";
    cin >> userType;
    cout << "Enter username: ";
    cin >> username;
    cout << "Enter password: ";
    cin >> password;
    cout << "Enter name: ";
    cin >> name;
    database.registerUser(userType, username, password, name);
}

void loginPage(Database& database) {
    string username, password;
    int userType, userId;
    cout << "Enter username: ";
    cin >> username;
    cout << "Enter password: ";
    cin >> password;
    if (database.loginUser(username, password, userType, userId)) {
        if (userType == 1) {
            adminMenu(database);
        }
        else {
            userMenu(database, userId);
        }
    }
    else {
        cout << "Invalid username or password!" << endl;
    }
}

int main() {
    int option;
    Database database;
    database.createDB();

    while (true) {
        welcomePage();
        cin >> option;
        if (option == 1) {
            loginPage(database);
        }
        else if (option == 2) {
            registerPage(database);
        }
        else if (option == 3) {
            break;
        }
        else {
            cout << "Invalid option!" << endl;
        }
    }

    return 0;
}
