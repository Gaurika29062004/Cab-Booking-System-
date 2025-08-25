#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <queue>
#include <string>
#include <iomanip>
#include <algorithm>
#include <limits>
#include <unordered_map>
#include <set>

using namespace std;

struct Cab {
    int id;
    string driver;
    int location;
    bool available;
    Cab(int id_=0, const string& driver_="", int loc_=0)
        : id(id_), driver(driver_), location(loc_), available(true) {}
};

struct Ride {
    int rideId;
    int cabId;
    int pickup, drop;
    int distance;
    int fare;
    vector<int> path;
    bool inProgress;
    Ride(int rideId_=0, int cabId_=0, int pickup_=0)
        : rideId(rideId_), cabId(cabId_), pickup(pickup_), drop(-1),
          distance(0), fare(0), inProgress(true) {}
};

class CabBookingSystem {
private:
    map<int, Cab> cabs;
    map<int, Ride> activeRides;
    vector<Ride> completedRides;
    map<int, vector<pair<int,int>>> graph;
    queue<int> rideQueue;  
    set<int> canceledCabs; // store cab IDs of canceled rides
    int nextRideId = 1;
    const string cabFile = "/Users/gaurikagupta/cab booking system/cabs.txt";

public:
    CabBookingSystem() { loadCabs(); }
    ~CabBookingSystem() { saveCabs(); }

    void resetAvailability() {
        for(auto &kv: cabs) kv.second.available = true;
        activeRides.clear();
        rideQueue = queue<int>();
        canceledCabs.clear();
        saveCabs();
        cout << "All cabs set to available.\n";
    }

    void addRoad(int u, int v, int w){
        graph[u].push_back({v,w});
        graph[v].push_back({u,w});
    }

    int dijkstra(int src, int dest, vector<int> &outPath){
        map<int,int> dist; map<int,int> prev;
        for(auto &kv: graph) dist[kv.first] = numeric_limits<int>::max();
        dist[src] = 0;
        priority_queue<pair<int,int>, vector<pair<int,int>>, greater<>> pq;
        pq.push({0, src});

        while(!pq.empty()){
            auto [d,u] = pq.top(); pq.pop();
            if(d>dist[u]) continue;
            for(auto &[v,w] : graph[u]){
                if(dist[v] > d + w){
                    dist[v] = d + w;
                    prev[v] = u;
                    pq.push({dist[v], v});
                }
            }
        }

        if(dist[dest]==numeric_limits<int>::max()) return -1;

        outPath.clear();
        for(int at = dest; at != src; at = prev[at]) outPath.push_back(at);
        outPath.push_back(src);
        reverse(outPath.begin(), outPath.end());
        return dist[dest];
    }

    bool addCab(int id, const string& driver, int location){
        if(cabs.count(id)) return false;
        cabs[id] = Cab(id, driver, location);
        saveCabs();
        return true;
    }

    void saveCabs(){
        ofstream fout(cabFile);
        for(auto &kv: cabs){
            Cab &c = kv.second;
            fout << c.id << " " << c.driver << " " << c.location << " " << c.available << "\n";
        }
    }

    void loadCabs(){
        ifstream fin(cabFile);
        if(!fin) return;
        int id, loc; string driver; bool avail;
        while(fin >> id >> driver >> loc >> avail){
            cabs[id] = Cab(id, driver, loc);
            cabs[id].available = avail;
        }
    }

    void listCabs() const {
        int w1 = 6, w2 = 18, w3 = 10, w4 = 12, w5 = 8;
        auto printLine = [&]() {
            cout << "+" << string(w1,'-') << "+"
                 << string(w2,'-') << "+"
                 << string(w3,'-') << "+"
                 << string(w4,'-') << "+"
                 << string(w5,'-') << "+\n";
        };

        unordered_map<int,int> cabToRide;
        for(auto &kv: activeRides) cabToRide[kv.second.cabId] = kv.second.rideId;

        printLine();
        cout << "|" << left << setw(w1) << "ID"
             << "|" << setw(w2) << "Driver"
             << "|" << setw(w3) << "Location"
             << "|" << setw(w4) << "Available"
             << "|" << setw(w5) << "RideID" << "|\n";
        printLine();

        for(auto &kv : cabs){
            const Cab &c = kv.second;
            bool isAvailable = c.available;
            int rideId = -1;
            string color;

            if(cabToRide.count(c.id)){
                rideId = cabToRide[c.id];
                isAvailable = false;
                color = "\033[1;31m"; // red
            } else if(canceledCabs.count(c.id)){
                color = "\033[1;35m"; // magenta
                rideId = -1;
                isAvailable = false;
            } else {
                rideId = -1;
                color = "\033[1;32m"; // green
            }

            cout << color;
            cout << "|" << left << setw(w1) << c.id
                 << "|" << setw(w2) << c.driver
                 << "|" << setw(w3) << c.location
                 << "|" << setw(w4) << (isAvailable?"Yes":"No")
                 << "|" << setw(w5) << (rideId==-1?"-":to_string(rideId)) << "|\n";
            cout << "\033[0m";
        }
        printLine();
    }

    void assignQueuedRideToCab(int cabId, int pickup) {
        if(graph.find(pickup) == graph.end()) {
            cout << "Pickup node invalid.\n";
            return;
        }

        Cab &chosen = cabs[cabId];
        chosen.available = false;

        vector<int> path;
        int dist = dijkstra(chosen.location, pickup, path);
        if(dist == -1) {
            cout << "Cab cannot reach pickup node.\n";
            return;
        }

        chosen.location = pickup;
        int thisRideId = nextRideId++;
        Ride r(thisRideId, chosen.id, pickup);
        r.path = path;
        activeRides[thisRideId] = r;

        canceledCabs.erase(chosen.id); // remove if it was magenta
        cout << "Queued ride " << thisRideId << " assigned to Cab " << chosen.id
             << " (Driver: " << chosen.driver << ") at Pickup Node " << pickup << ".\n";

        saveCabs();
    }

    void bookRide(int pickup){
        if(graph.find(pickup)==graph.end()){ cout<<"Pickup node invalid.\n"; return; }
        int bestId=-1, bestDist=numeric_limits<int>::max(); vector<int> bestPath;
        for(auto &kv:cabs){
            Cab &c=kv.second;
            if(!c.available) continue;
            vector<int> path; int dist=dijkstra(c.location, pickup, path);
            if(dist==-1) continue;
            if(dist<bestDist || (dist==bestDist && c.id<bestId)){
                bestDist=dist; bestId=c.id; bestPath=path;
            }
        }
        if(bestId==-1){ 
            cout<<"No available cabs. Ride queued.\n"; 
            rideQueue.push(pickup);
            return; 
        }

        Cab &chosen = cabs[bestId];
        chosen.available = false;
        chosen.location = pickup;
        int thisRideId = nextRideId++;
        Ride r(thisRideId, chosen.id, pickup);
        r.path = bestPath;
        activeRides[thisRideId] = r;

        canceledCabs.erase(chosen.id);
        cout<<"Ride "<<thisRideId<<" booked with Cab "<<chosen.id<<" (Driver: "<<chosen.driver<<") at Pickup Node "<<pickup<<".\n";
        saveCabs();
    }

    void endRide(int rideId, int drop){
    if(!activeRides.count(rideId)){ cout<<"Invalid RideID.\n"; return; }
    if(graph.find(drop)==graph.end()){ cout<<"Drop node invalid.\n"; return; }

    Ride r = activeRides[rideId];
    vector<int> path;
    int dist = dijkstra(r.pickup, drop, path);
    if(dist==-1){ cout<<"Pickup & Drop not connected.\n"; return; }

    int fare = max(0, dist*10);
    r.drop=drop; r.distance=dist; r.fare=fare; r.path=path; r.inProgress=false;

    completedRides.push_back(r);
    activeRides.erase(rideId);

    Cab &c = cabs[r.cabId];
    c.location = drop;

    if(!rideQueue.empty()){
        int nextPickup = rideQueue.front();
        rideQueue.pop();
        cout << "\033[1;36mAssigning queued ride at pickup " << nextPickup << " to cab " << c.id << "\033[0m\n";
        assignQueuedRideToCab(c.id, nextPickup);
    } else {
        c.available = true;
        cout << "\033[1;32mCab " << c.id << " is now available.\033[0m\n";  // Green highlight
    }

    saveCabs();
    cout<<"Ride "<<rideId<<" ended. Cab "<<c.id<<" now available.\nDrop Node: "<<drop<<"\nDistance: "<<dist<<"\nFare: Rs "<<fare<<"\n";
}


    void cancelRide(int rideId){
    if(!activeRides.count(rideId)){ 
        cout<<"Invalid RideID.\n"; 
        return; 
    }
    Ride r = activeRides[rideId];
    activeRides.erase(rideId);

    Cab &c = cabs[r.cabId];
    canceledCabs.insert(c.id);

    cout << "\033[1;35mRide " << rideId << " canceled.";

    if(!rideQueue.empty()){
        int nextPickup = rideQueue.front();
        rideQueue.pop();
        cout << "\033[1;36mAssigning queued ride at pickup " << nextPickup << " to cab " << c.id << "\033[0m\n";
        assignQueuedRideToCab(c.id, nextPickup);
    } else {
        c.available = true;
        cout << "\033[1;32mCab " << c.id << " is now available.\033[0m\n";  // Green highlight
    }

    saveCabs();
}



    void listActiveRides() const {
        if(activeRides.empty()){ cout<<"No active rides.\n"; return; }
        int w1=8,w2=8,w3=8,w4=8,w5=8,w6=12,w7=25;
        auto printLine=[&](){ cout<<"+"<<string(w1,'-')<<"+"<<string(w2,'-')<<"+"<<string(w3,'-')<<"+"<<string(w4,'-')<<"+"<<string(w5,'-')<<"+"<<string(w6,'-')<<"+"<<string(w7,'-')<<"+\n"; };
        printLine();
        cout<<"|"<<left<<setw(w1)<<"RideID"<<"|"<<setw(w2)<<"CabID"<<"|"<<setw(w3)<<"Pickup"<<"|"<<setw(w4)<<"Drop"<<"|"<<setw(w5)<<"Dist"<<"|"<<setw(w6)<<"InProgress"<<"|"<<setw(w7)<<"Path"<<"|\n";
        printLine();
        for(auto &kv: activeRides){
            const Ride &r = kv.second;
            cout<<"|"<<left<<setw(w1)<<r.rideId<<"|"<<setw(w2)<<r.cabId<<"|"<<setw(w3)<<r.pickup<<"|"<<setw(w4)<<(r.drop==-1?0:r.drop)<<"|"<<setw(w5)<<r.distance<<"|"<<setw(w6)<<(r.inProgress?"Yes":"No")<<"|";
            string pathStr; for(size_t i=0;i<r.path.size();i++){ pathStr+=to_string(r.path[i]); if(i!=r.path.size()-1) pathStr+="->"; }
            if(pathStr.size()>w7-1) pathStr=pathStr.substr(0,w7-4)+"...";
            cout<<left<<setw(w7)<<pathStr<<"|\n"; printLine();
        }
    }

    void listCompletedRides() const {
        if(completedRides.empty()){ cout<<"No completed rides.\n"; return; }
        int w1=8,w2=8,w3=8,w4=8,w5=8,w6=8,w7=25;
        auto printLine=[&](){ cout<<"+"<<string(w1,'-')<<"+"<<string(w2,'-')<<"+"<<string(w3,'-')<<"+"<<string(w4,'-')<<"+"<<string(w5,'-')<<"+"<<string(w6,'-')<<"+"<<string(w7,'-')<<"+\n"; };
        printLine();
        cout<<"|"<<left<<setw(w1)<<"RideID"<<"|"<<setw(w2)<<"CabID"<<"|"<<setw(w3)<<"Pickup"<<"|"<<setw(w4)<<"Drop"<<"|"<<setw(w5)<<"Dist"<<"|"<<setw(w6)<<"Fare"<<"|"<<setw(w7)<<"Path"<<"|\n";
        printLine();
        for(auto &r: completedRides){
            cout<<"|"<<left<<setw(w1)<<r.rideId<<"|"<<setw(w2)<<r.cabId<<"|"<<setw(w3)<<r.pickup<<"|"<<setw(w4)<<r.drop<<"|"<<setw(w5)<<r.distance<<"|"<<setw(w6)<<r.fare<<"|";
            string pathStr; for(size_t i=0;i<r.path.size();i++){ pathStr+=to_string(r.path[i]); if(i!=r.path.size()-1) pathStr+="->"; }
            if(pathStr.size()>w7-1) pathStr=pathStr.substr(0,w7-4)+"...";
            cout<<left<<setw(w7)<<pathStr<<"|\n"; printLine();
        }
    }

    void listQueuedRides() const {
        if(rideQueue.empty()){ cout<<"No rides waiting in the queue.\n"; return; }
        int w1 = 6, w2 = 15;
        auto printLine = [&]() {
            cout << "+" << string(w1,'-') << "+" << string(w2,'-') << "+\n";
        };
        printLine();
        cout << "|" << left << setw(w1) << "Pos"
             << "|" << setw(w2) << "Pickup Node" << "|\n";
        printLine();

        queue<int> temp = rideQueue;
        int pos = 1;
        while(!temp.empty()){
            cout << "|" << left << setw(w1) << pos
                 << "|" << setw(w2) << temp.front() << "|\n";
            temp.pop(); pos++;
        }
        printLine();
    }
};

int main(){
    CabBookingSystem app;
    app.resetAvailability();

    for(int i=0;i<=9;i++)
        for(int j=i+1;j<=9;j++)
            app.addRoad(i,j,abs(i-j));

    while(true){
        cout<<"\n=== Cab Booking System ===\n";
        cout<<"1. Add Cab\n2. List Cabs\n3. Book Ride\n4. List Active Rides\n5. End Ride\n6. List Completed Rides\n7. Cancel Ride\n8. List Queued Rides\n0. Exit\nChoose: ";
        int ch; if(!(cin>>ch)) break;
        if(ch==0) break;
        else if(ch==1){
            int id,loc; string driver;
            cout<<"Enter Cab ID: "; cin>>id;
            cout<<"Enter Driver Name: "; cin>>driver;
            cout<<"Enter Start Node: "; cin>>loc;
            if(app.addCab(id,driver,loc)) cout<<"Cab added.\n";
            else cout<<"Cab ID already exists.\n";
        }
        else if(ch==2) app.listCabs();
        else if(ch==3){
            int pickup; cout<<"Enter Pickup Node: "; cin>>pickup;
            app.bookRide(pickup);
        }
        else if(ch==4) app.listActiveRides();
        else if(ch==5){
            int rideId, drop; cout<<"Enter Ride ID: "; cin>>rideId;
            cout<<"Enter Drop Node: "; cin>>drop;
            app.endRide(rideId,drop);
        }
        else if(ch==6) app.listCompletedRides();
        else if(ch==7){
            int rideId; cout<<"Enter Ride ID to cancel: "; cin>>rideId;
            app.cancelRide(rideId);
        }
        else if(ch==8) app.listQueuedRides();
        else cout<<"Invalid choice.\n";
    }
    return 0;
}
