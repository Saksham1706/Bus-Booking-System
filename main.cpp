#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <set>
#include <string>
#include <algorithm>
#include <ctime>

#ifdef _WIN32
#include <windows.h>
#endif

using namespace std;

// === Console UI Utilities (Classic C-like Color Macros) ===

namespace UI {
    void enableVirtualTerminal() {
#ifdef _WIN32
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        if(hOut!=INVALID_HANDLE_VALUE) {
            DWORD dwMode=0;
            if(GetConsoleMode(hOut, &dwMode)) {
                SetConsoleMode(hOut, dwMode|0x0004);
            }
        }
#endif
    }
    void clearScreen() {
#ifdef _WIN32
        system("cls");
#else
        system("clear");
#endif
    }
    string RED   () { return "\033[1;31m"; }
    string GREEN () { return "\033[1;32m"; }
    string CYAN  () { return "\033[1;36m"; }
    string YELLOW() { return "\033[1;33m"; }
    string BOLD  () { return "\033[1m"; }
    string RESET () { return "\033[0m"; }
    void line() { cout << CYAN() << string(90, '=')    << RESET() << "\n"; }
    void boxHead(const char* title) {
        line();
        cout << CYAN() << "| " << std::left << std::setw(86) << title << "|\n" << RESET();
        line();
    }
    void pressEnter() {
        cout << YELLOW() << "Press Enter to continue..." << RESET();
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

// === Seat Classes ===
enum class SeatClass { Regular, Deluxe, VIP };
string classToStr(SeatClass sc) {
    if(sc==SeatClass::VIP) return UI::CYAN()+"VIP   "+UI::RESET();
    if(sc==SeatClass::Deluxe) return UI::YELLOW()+"Deluxe"+UI::RESET();
    return UI::GREEN()+"Regular"+UI::RESET();
}
int classToColor(SeatClass sc) {
    if(sc==SeatClass::VIP) return 36;
    if(sc==SeatClass::Deluxe) return 33;
    return 32;
}

// === Data Structures ===
struct User {
    string username, password;
    bool isAdmin;
    User(string u="",string p="",bool a=false):username(u),password(p),isAdmin(a){}
};
struct Reservation {
    string username;
    int busNo, seat;
    string passenger;
    string date;
    bool operator<(const Reservation& rhs) const {
        return tie(date,busNo,seat,username) < tie(rhs.date,rhs.busNo,rhs.seat,rhs.username);
    }
};
struct SeatInfo {
    bool booked = false;
    string whoBooked;
    SeatClass seatClass = SeatClass::Regular;
};
class Bus {
public:
    int number, fare;
    string name, dest, time;
    Bus(int n, string nm, string d, int f, string t)
      : number(n), fare(f), name(nm), dest(d), time(t){}
    map<string, vector<SeatInfo>> seatsByDate;

    void setupSeatClasses(vector<SeatInfo>& seats) {
        seats.resize(33);
        for(int i=1;i<=32;++i) {
            if(i>=1&&i<=4) seats[i].seatClass=SeatClass::VIP;
            else if(i>=5&&i<=12) seats[i].seatClass=SeatClass::Deluxe;
            else seats[i].seatClass=SeatClass::Regular;
        }
    }
    void ensureDate(const string& jd) {
        if(!seatsByDate.count(jd))
            setupSeatClasses(seatsByDate[jd]);
    }
    bool isAvailable(const string& jd, int seat) {
        ensureDate(jd);
        return seat>=1 && seat<=32 && !seatsByDate[jd][seat].booked;
    }
    SeatClass getSeatClass(int seat) {
        if(seat>=1&&seat<=4) return SeatClass::VIP;
        if(seat>=5&&seat<=12) return SeatClass::Deluxe;
        return SeatClass::Regular;
    }
    void printSeats(const string& jd) {
        ensureDate(jd);
        cout << UI::BOLD() << UI::CYAN() << "\n         ----------- Bus Seat Layout -----------\n" << UI::RESET();
        for(int row=0;row<8;++row) {
            cout << "       ";
            for(int col=1;col<=4;++col) {
                int seat = row*4+col;
                SeatClass sc = seatsByDate[jd][seat].seatClass;
                std::string colorseq = "\033["+to_string(classToColor(sc))+"m";
                bool bkd = seatsByDate[jd][seat].booked;
                if(bkd) cout << UI::RED() ;
                else cout << colorseq;
                cout << "[" << std::setw(2) << setfill('0') << seat << "]";
                cout << UI::RESET();
                cout << " ";
            }
            cout << endl;
        }
        cout << "\n     " << UI::RED() << "[NN]" << UI::RESET()
             << "=Booked  " << UI::CYAN() << "[NN]" << UI::RESET() << "=VIP  "
             << UI::YELLOW() << "[NN]" << UI::RESET() << "=Deluxe  "
             << UI::GREEN() << "[NN]" << UI::RESET() << "=Regular\n";
    }
    bool book(const string& date, int s, const string& uname) {
        ensureDate(date);
        if(!isAvailable(date,s)) return false;
        seatsByDate[date][s].booked = true;
        seatsByDate[date][s].whoBooked = uname;
        return true;
    }
    void cancel(const string& date, int s) {
        ensureDate(date);
        seatsByDate[date][s].booked=false;
        seatsByDate[date][s].whoBooked="";
    }
};

// === Globals ===
vector<User> users;
vector<Bus> buses;
set<Reservation> reservations;

// === Load/Save ===
void loadUsers() {
    users.clear();
    ifstream in("users.txt");
    string u,p;
    while(in>>u>>p) users.emplace_back(u,p,(u=="admin"));
    in.close();
    if(find_if(users.begin(),users.end(),[](const User& u){return u.username=="admin";})==users.end())
        users.emplace_back("admin","admin",true);
}
void saveUsers() {
    ofstream out("users.txt");
    for(const User& user:users) out<<user.username<<" "<<user.password<<endl;
    out.close();
}
void loadReservations() {
    reservations.clear();
    ifstream in("reservations.txt");
    string user,date,pass; int bus,seat;
    while(in>>user>>date>>bus>>seat>>pass)
        reservations.insert(Reservation{user,bus,seat,pass,date});
    in.close();
}
void saveReservations() {
    ofstream out("reservations.txt");
    for(const auto& r:reservations)
        out<<r.username<<" "<<r.date<<" "<<r.busNo<<" "<<r.seat<<" "<<r.passenger<<endl;
    out.close();
}

// === Init Buses/Seats ===
void initBuses() {
    const vector<string> names = {
        "Chintamni Travels","Prabhat Travels","Jhakar Travels","Guru Ganesh","Tayal Tours",
        "Rajhans","Uncle Travels","Royal Rajasthan","RSRTC"
    }, dests = {
        "Nagpur to Yeotmal", "Jodhpur To Jaipur", "Jodhpur To New Delhi", "Nagpur To Pune",
        "Nagpur To Indore", "Nagpur to Jodhpur", "Jodhpur To Indore", "Jodhpur To Jaisalmer", "Jodhpur To Nagpur"
    };
    const vector<int> fares={320,350,750,900,650,2600,1100,520,3200};
    const vector<string> times = {"07:00 AM","01:30 PM","03:50 PM","01:00 AM","12:05 AM","09:30 PM","11:00 PM","08:15 AM","08:00 PM"};
    buses.clear();
    for(int i=0;i<9;++i)
        buses.emplace_back(i+1,names[i],dests[i],fares[i],times[i]);
}
void fillBusSeatsFromReservations() {
    for(auto& bus : buses) bus.seatsByDate.clear();
    for(const auto& r:reservations) {
        if(r.busNo>=1 && r.busNo<=(int)buses.size()) {
            buses[r.busNo-1].ensureDate(r.date);
            buses[r.busNo-1].seatsByDate[r.date][r.seat].booked = true;
            buses[r.busNo-1].seatsByDate[r.date][r.seat].whoBooked = r.username;
            buses[r.busNo-1].seatsByDate[r.date][r.seat].seatClass = buses[r.busNo-1].getSeatClass(r.seat);
        }
    }
}

// === Date Helpers ===
bool parseDate(const string& str, std::tm& tmout) {
    std::istringstream ss(str);
    ss >> std::get_time(&tmout, "%Y-%m-%d");
    if(ss.fail()) return false;
    tmout.tm_hour = 0; tmout.tm_min = 0; tmout.tm_sec = 0;
    return true;
}
std::tm todayDate() {
    time_t t = time(nullptr);
    std::tm now = *localtime(&t); // <- always portable (use this for any OS)
    now.tm_hour=0; now.tm_min=0; now.tm_sec=0;
    return now;
}
int compareDays(const std::tm& a, const std::tm& b) {
    if(a.tm_year!=b.tm_year) return a.tm_year-b.tm_year;
    if(a.tm_mon!=b.tm_mon) return a.tm_mon-b.tm_mon;
    return a.tm_mday-b.tm_mday;
}
int daysBetween(const std::tm& earlier, const std::tm& later) {
    std::tm early=earlier, late=later;
    std::time_t t1=mktime(&early), t2=mktime(&late);
    return int((t2-t1)/(60*60*24));
}

// === Accounts/Login ===
User* loginMenu(string& activeUser) {
    UI::clearScreen(); UI::boxHead("Login");
    string u,p;
    cout<<"Username: "; getline(cin,u);
    cout<<"Password: "; getline(cin,p);
    for(User& user : users) {
        if(user.username==u && user.password==p) {
            activeUser = user.username;
            cout<<UI::GREEN()<<"Login successful!\n"<<UI::RESET(); UI::pressEnter();
            return &user;
        }
    }
    cout<<UI::RED()<<"Invalid username/password!\n"<<UI::RESET(); UI::pressEnter();
    return nullptr;
}
void signupMenu() {
    UI::clearScreen(); UI::boxHead("Signup");
    string u,p;
    cout<<"Choose username: "; getline(cin,u);
    if(find_if(users.begin(),users.end(),[&](const User& us){return us.username==u;})!=users.end()) {
        cout<<UI::RED()<<"Username already exists!\n"<<UI::RESET(); UI::pressEnter();
        return;
    }
    cout<<"Choose password: "; getline(cin,p);
    users.emplace_back(u,p,false); saveUsers();
    cout<<UI::GREEN()<<"Signup successful.\n"<<UI::RESET(); UI::pressEnter();
}

// === User Menus ===
void showBuses() {
    UI::clearScreen(); UI::boxHead("Available Buses");
    cout<<UI::BOLD()<<setw(6)<<"Bus#" << setw(23) << "Name" << setw(28) << "Destination"
        << setw(10) << "Charge" << setw(10) << "Time" << UI::RESET() << "\n";
    UI::line();
    for(const auto& bus: buses)
        cout<<setw(6)<<bus.number<<setw(23)<<bus.name.substr(0,22)<<setw(28)<<bus.dest.substr(0,27)
        <<setw(10)<<bus.fare<<setw(10)<<bus.time<<"\n";
    UI::line(); UI::pressEnter();
}
void seatStatus(const string&) {
    UI::clearScreen(); UI::boxHead("Seat Status");
    int bn; string dt;
    showBuses();
    cout<<"Choose bus number (1-9): "; cin>>bn; cin.ignore();
    if(bn<1||bn>9){cout<<UI::RED()<<"Invalid bus number\n"<<UI::RESET();UI::pressEnter();return;}
    cout<<"Enter Date (YYYY-MM-DD): "; getline(cin,dt);
    std::tm jdate;
    if(!parseDate(dt,jdate)){cout<<UI::RED()<<"Invalid date format.\n"<<UI::RESET();UI::pressEnter();return;}
    buses[bn-1].printSeats(dt); UI::pressEnter();
}
void bookTicket(const string& uname) {
    UI::clearScreen();
    showBuses();
    cout<<"Enter bus number (1-9): "; int bn; cin>>bn; cin.ignore();
    if(bn<1||bn>9){cout<<UI::RED()<<"Invalid bus.\n"<<UI::RESET();UI::pressEnter();return;}
    cout<<"Enter date of journey (YYYY-MM-DD): "; string dt; getline(cin,dt);

    std::tm jdate;
    if(!parseDate(dt, jdate)) {
        cout<<UI::RED()<<"Invalid date format. Please use YYYY-MM-DD.\n"<<UI::RESET(); UI::pressEnter(); return;
    }
    std::tm today = todayDate();
    int diff = compareDays(jdate, today);
    if(diff < 0) {
        cout<<UI::RED()<<"Cannot book for a previous date!\n"<<UI::RESET(); UI::pressEnter(); return;
    }
    if(daysBetween(today, jdate) > 60) {
        cout<<UI::RED()<<"Bookings only up to 2 months from today.\n"<<UI::RESET(); UI::pressEnter(); return;
    }

    buses[bn-1].printSeats(dt);
    cout<<"Choose seat number (1-32): "; int sn; cin>>sn; cin.ignore();
    if(sn<1||sn>32){cout<<UI::RED()<<"Invalid seat.\n"<<UI::RESET();UI::pressEnter();return;}
    if(!buses[bn-1].isAvailable(dt,sn)){cout<<UI::RED()<<"Seat booked.\n"<<UI::RESET();UI::pressEnter();return;}
    string pass; cout<<"Enter passenger first name: "; getline(cin,pass);
    buses[bn-1].book(dt,sn,uname);
    reservations.insert({uname,bn,sn,pass,dt});
    saveReservations(); fillBusSeatsFromReservations();
    cout<<UI::GREEN()<<"Ticket booked! Bus: "<<bn<<", Date: "<<dt<<", Seat: "<<sn<<UI::RESET()<<"\n"; UI::pressEnter();
}
void myReservations(const string& uname) {
    UI::clearScreen(); UI::boxHead("Your Reservations");
    cout<<UI::BOLD()<<setw(6)<<"Bus#" << setw(13)<<"Date"<<setw(8)<<"Seat"<<setw(11)
            <<"Class"<<setw(15)<<"Passenger"<<UI::RESET()<<"\n";
    UI::line();
    int cnt=0;
    for(const auto& r:reservations) {
        if(r.username==uname) {
            SeatClass c = buses[r.busNo-1].getSeatClass(r.seat);
            cout<<setw(6)<<r.busNo<<setw(13)<<r.date<<setw(8)<<r.seat
                <<setw(11)<<classToStr(c)<<setw(15)<<r.passenger<<"\n"; ++cnt;
        }
    }
    if(!cnt) cout<<"No reservations.\n";
    UI::line(); UI::pressEnter();
}
void cancelReservation(const string& uname) {
    UI::clearScreen(); UI::boxHead("Cancel Reservation");
    myReservations(uname);
    cout<<"Bus no to cancel: "; int bn; cin>>bn; cin.ignore();
    cout<<"Date (YYYY-MM-DD): "; string dt; getline(cin,dt);
    cout<<"Seat to cancel: "; int sn; cin>>sn; cin.ignore();
    auto it = reservations.find({uname,bn,sn,"",dt});
    if(it==reservations.end()){cout<<UI::RED()<<"No such reservation.\n"<<UI::RESET();UI::pressEnter();return;}
    buses[bn-1].cancel(dt,sn);
    reservations.erase(it);
    saveReservations(); fillBusSeatsFromReservations();
    cout<<UI::GREEN()<<"Cancelled successfully.\n"<<UI::RESET(); UI::pressEnter();
}
void reservationInfo() {
    UI::clearScreen(); UI::boxHead("Reservation Info");
    cout<<"Enter date (YYYY-MM-DD): "; string dt; getline(cin,dt);
    cout<<"Enter bus number: "; int bn; cin>>bn;
    cout<<"Seat number: "; int sn; cin>>sn; cin.ignore();
    bool found=false;
    for(const auto& r:reservations)
        if(r.date==dt && r.busNo==bn && r.seat==sn) {
            cout<<"Passenger: "<<r.passenger<<", User: "<<r.username<<"\n";
            found=true;
        }
    if(!found) cout<<UI::RED()<<"No reservation.\n"<<UI::RESET();
    UI::pressEnter();
}

// === Admin Panel ===
void listAllBookings() {
    UI::clearScreen(); UI::boxHead("ALL BOOKINGS");
    cout<<UI::BOLD()<<setw(6)<<"Bus#" << setw(13)<<"Date"<<setw(8)<<"Seat"<<setw(11)
            <<"Class"<<setw(12)<<"User"<<setw(15)<<"Name"<<UI::RESET()<<"\n";
    UI::line();
    for(const auto& r:reservations) {
        SeatClass cc = buses[r.busNo-1].getSeatClass(r.seat);
        cout<<setw(6)<<r.busNo<<setw(13)<<r.date<<setw(8)<<r.seat<<setw(11)<<classToStr(cc)
            <<setw(12)<<r.username<<setw(15)<<r.passenger<<"\n";
    }
    UI::line(); UI::pressEnter();
}
void showStats() {
    UI::clearScreen(); UI::boxHead("STATISTICS");
    map<string,int> dayCount;
    map<int,int> busBookings;
    for(const auto& r:reservations) {
        dayCount[r.date]++;
        busBookings[r.busNo]++;
    }
    cout<<"Bookings Per Day:\n";
    for(const auto& p:dayCount)
        cout<<"  "<<p.first<<" : "<<p.second<<"\n";
    cout<<"\nBookings Per Bus:\n";
    for(const auto& p:busBookings)
        cout<<"  Bus "<<p.first<<" : "<<p.second<<"\n";
    UI::pressEnter();
}
void manageUsers() {
    UI::clearScreen(); UI::boxHead("User Management");
    cout<<"Users:\n";
    for(int i=0;i<(int)users.size();++i)
        cout<<"  "<<i+1<<". "<<users[i].username<<(users[i].isAdmin?" (admin)":"")<<"\n";
    cout<<"Enter user# to remove, or 0 to return: "; int n; cin>>n; cin.ignore();
    if(n==0) return;
    if(n>=1 && n<=users.size() && !users[n-1].isAdmin) {
        string u=users[n-1].username;
        users.erase(users.begin()+(n-1));
        saveUsers();
        for(auto it=reservations.begin(); it!=reservations.end(); )
            if(it->username==u) it=reservations.erase(it);
            else ++it;
        saveReservations(); fillBusSeatsFromReservations();
        cout<<"Deleted.\n";
    } else cout<<"Not allowed.\n";
    UI::pressEnter();
}
void adminMenu() {
    int c;
    do {
        UI::clearScreen(); UI::boxHead("ADMIN PANEL");
        cout<<" 1. View all bookings\n 2. Statistics\n 3. Manage users\n 4. Logout\n\nChoice: ";
        cin>>c; cin.ignore();
        switch(c) {
            case 1: listAllBookings(); break;
            case 2: showStats(); break;
            case 3: manageUsers(); break;
            case 4: return;
        }
    } while(1);
}

// === Main App ===
void mainMenu() {
    string actUser; User* logged=nullptr;
    while(1) {
        UI::clearScreen();
        UI::boxHead("WELCOME TO ONLINE BUS RESERVATION");
        cout << " 1. Login\n 2. Signup\n 3. Exit\n\nChoice: ";
        int c; cin>>c; cin.ignore();
        if(c==1) {
            logged = loginMenu(actUser);
            if(logged==nullptr) continue;
            fillBusSeatsFromReservations();
            if(logged->isAdmin)
                adminMenu();
            else {
                while(true) {
                    UI::clearScreen(); UI::boxHead("Main Menu");
                    cout << " 1. Show Bus List\n 2. Status of Seats\n 3. Book Ticket\n 4. Cancel Reservation\n"
                         << " 5. My Reservations\n 6. Reservation Info (by seat/date)\n 7. Logout\n\nChoice: ";
                    int ch; cin>>ch; cin.ignore();
                    switch(ch) {
                        case 1: showBuses(); break;
                        case 2: seatStatus(actUser); break;
                        case 3: bookTicket(actUser); break;
                        case 4: cancelReservation(actUser); break;
                        case 5: myReservations(actUser); break;
                        case 6: reservationInfo(); break;
                        case 7: goto main_menu_logout;
                    }
                }
                main_menu_logout:;
            }
        }
        else if(c==2) signupMenu();
        else if(c==3) {
            UI::line(); cout << UI::GREEN() << "\nTHANK YOU FOR USING OUR SERVICE....\n" << UI::RESET();
            UI::line(); exit(0);
        }
    }
}

int main() {
    ios::sync_with_stdio(false);
    UI::enableVirtualTerminal();
    initBuses(); loadUsers(); loadReservations(); fillBusSeatsFromReservations();
    mainMenu();
    return 0;
}
