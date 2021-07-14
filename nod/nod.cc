#include <iostream>
#include <map>
#include <regex>
#include <vector>

#define ADD_INFO_REGEX R"(\s*[A-Za-z0-9]{3,11}\s+[AS][1-9][\d]{0,2}\s+([0]|[1-9]\d*)[,]\d\s*)"
#define GET_INFO R"(\s*[?]\s*)"
#define CAR_INFO R"(\s*[?]\s*[A-Za-z0-9]{3,11}\s*)"
#define ROAD_INFO R"(\s*[?]\s*[AS][1-9]\d{0,2}\s*)"
#define EMPTY_LINE R"([\s]*)"

using namespace std;

using Dist = pair<int64_t , int>;
using RoadId = pair<char, int>;
using SingleCarInfo = pair<string, pair<Dist, Dist>>;
using AllCarsInfo = map<string, pair<Dist, Dist>>;
using SingleRoadInfo = pair<RoadId, Dist>;
using RoadsInfo = map<RoadId, Dist>;
using LineInfo = pair<string, uint64_t>;
using CarsEntered = vector<LineInfo>;

void printError(const LineInfo &line) {
    cerr << "Error in line " << line.second
         << ": " << line.first << endl;
}

bool isCarOnRoad(string &car, LineInfo &prevLine,
                 CarsEntered &carsOnRoad) {
    string registration;
    for (auto const &c: carsOnRoad) {
        stringstream stream(c.first);
        stream >> registration;
        if (registration == car) {
            prevLine = c;
            return true;
        }
    }
    return false;
}

Dist distDiff(Dist &dist1, Dist &dist2) {
    int64_t km1 = dist1.first, km2 = dist2.first;
    int meters1 = dist1.second, meters2 = dist2.second;

    if (km1 > km2) {
        if (meters1 < meters2)
            return make_pair(km1 - km2 - 1,
                             meters1 + 10 - meters2);
        else
            return make_pair(km1 - km2, meters1 - meters2);
    } else if (km1 == km2) {
        if (meters1 < meters2)
            return make_pair(0, meters2 - meters1);
        else
            return make_pair(0, meters1 - meters2);
    } else {
        if (meters1 > meters2)
            return make_pair(km2 - km1 - 1,
                             meters2 + 10 - meters1);
        else
            return make_pair(km2 - km1, meters2 - meters1);
    }
}

Dist distSum(Dist &dist1, Dist &dist2) {
    int64_t km1 = dist1.first, km2 = dist2.first;
    int meters1 = dist1.second, meters2 = dist2.second;

    if (km1 == -1) {
        km1 = 0;
    }

    if (meters1 + meters2 < 10)
        return make_pair(km1 + km2, meters1 + meters2);
    else
        return make_pair(km1 + km2 + 1,
                         meters1 + meters2 - 10);
}

void updateCars(AllCarsInfo &cars, string &registration,
                RoadId &id, Dist &distance) {
    Dist zero = make_pair(-1, 0);
    auto carIterator = cars.find(registration);

    if (carIterator == cars.end()) {
        if (id.first == 'A') {
            cars.insert(make_pair(registration,
                                  make_pair(distance, zero)));
        } else {
            cars.insert(make_pair(registration,
                                  make_pair(zero, distance)));
        }
    } else {
        if (id.first == 'A') {
            carIterator->second.first
            = distSum(carIterator->second.first, distance);
        } else {
            carIterator->second.second
            = distSum(carIterator->second.second, distance);
        }
    }
}

void updateRoads(RoadsInfo &roads, RoadId &id,
                 Dist &distance) {
    auto roadIterator = roads.find(id);

    if (roadIterator == roads.end()) {
        roads.insert(make_pair(id, distance));
    } else {
        roadIterator->second = distSum(roadIterator->second,
                                       distance);
    }
}

void addInfo(AllCarsInfo &cars, RoadsInfo &roads,
             string &registration, RoadId &id,
             Dist &distance) {
    updateCars(cars, registration, id, distance);
    updateRoads(roads, id, distance);
}

void deleteLine(CarsEntered &carsOnRoad, LineInfo &line) {
    size_t size = carsOnRoad.size();

    for (size_t i = 0; i < size; i++) {
        if (carsOnRoad[i] == line) {
            carsOnRoad.erase(carsOnRoad.begin() + i);
            break;
        }
    }
}

void newInfo(AllCarsInfo &cars, RoadsInfo &roads,
             CarsEntered &carsOnRoad, LineInfo &line) {
    RoadId id1, id2;
    string registration, dumpS;
    char dumpC;
    Dist dist1, dist2, dist3;
    LineInfo prevLine;
    stringstream stream1(line.first);
    stream1 >> registration >> id1.first >> id1.second
            >> dist1.first >> dumpC >> dist1.second;

    if (isCarOnRoad(registration, prevLine, carsOnRoad)) {
        stringstream stream2(prevLine.first);
        stream2 >> dumpS >> id2.first >> id2.second
                >> dist2.first >> dumpC >> dist2.second;

        if (id1 == id2) {
            dist3 = distDiff(dist1, dist2);
            addInfo(cars, roads, registration, id1, dist3);
        } else {
            printError(prevLine);
            carsOnRoad.push_back(line);
        }
        deleteLine(carsOnRoad, prevLine);
    } else {
        carsOnRoad.push_back(line);
    }
}

void printCar(const SingleCarInfo &car) {
    Dist distA = car.second.first;
    Dist distS = car.second.second;
    cout << car.first;

    if (distA.first != -1)
        cout << " A " << distA.first << "," << distA.second;

    if (distS.first != -1)
        cout << " S " << distS.first << "," << distS.second;

    cout << endl;
}

void printRoad(const SingleRoadInfo &road) {
    RoadId id = road.first;
    Dist dist = road.second;
    cout << id.first << id.second << " " << dist.first
         << ',' << dist.second << endl;
}

void carInfo(AllCarsInfo &cars, string &line) {
    stringstream stream1(line);
    char toIgnore;
    string registration;

    stream1 >> toIgnore >> registration;
    auto carIterator = cars.find(registration);

    if (carIterator != cars.end()) {
        auto car = make_pair(carIterator->first,
                             carIterator->second);
        printCar(car);
    }
}

void roadInfo(RoadsInfo &roads, string &line) {
    stringstream stream1(line);
    char toIgnore;
    RoadId id;

    stream1 >> toIgnore >> id.first >> id.second;
    auto roadIterator = roads.find(id);

    if (roadIterator != roads.end()) {
        auto road = make_pair(roadIterator->first,
                              roadIterator->second);
        printRoad(road);
    }
}

void carAndRoadInfo(AllCarsInfo &cars, RoadsInfo &roads,
                    string &line) {
    carInfo(cars, line);
    roadInfo(roads, line);
}

bool carCompare(SingleCarInfo &i, SingleCarInfo &j) {
    return i.first < j.first;
}

bool roadCompare(SingleRoadInfo &road1,
                 SingleRoadInfo &road2) {
    RoadId id1 = road1.first;
    RoadId id2 = road2.first;
    if (id1.second == id2.second)
        return id1.first < id2.first;

    return id1.second < id2.second;
}

void allCarAndRoadInfo(AllCarsInfo &cars, RoadsInfo &roads) {
    vector<SingleCarInfo> carsVector(cars.begin(),
                                     cars.end());
    sort(carsVector.begin(), carsVector.end(), carCompare);
    for (auto const &car: carsVector)
        printCar(car);

    vector<SingleRoadInfo> roadsVector(roads.begin(),
                                       roads.end());
    sort(roadsVector.begin(), roadsVector.end(), roadCompare);
    for (auto const &road: roadsVector)
        printRoad(road);
}

void checkLine(AllCarsInfo &cars, RoadsInfo &roads,
               CarsEntered &carsOnRoad,
               LineInfo &currentLine) {
    string line = currentLine.first;
    if (regex_match(line, regex(ADD_INFO_REGEX))) {
        newInfo(cars, roads, carsOnRoad, currentLine);
    } else if (regex_match(line, regex(GET_INFO))) {
        allCarAndRoadInfo(cars, roads);
    } else if (regex_match(line, regex(CAR_INFO)) &&
               regex_match(line, regex(ROAD_INFO))) {
        carAndRoadInfo(cars, roads, line);
    } else if (regex_match(line, regex(CAR_INFO))) {
        carInfo(cars, line);
    } else if (regex_match(line, regex(ROAD_INFO))) {
        roadInfo(roads, line);
    } else if (!regex_match(line, regex(EMPTY_LINE))) {
        printError(currentLine);
    }
}

int main() {
    uint64_t lineCounter = 1;
    string inputLine;
    AllCarsInfo cars;
    RoadsInfo roads;
    CarsEntered carsOnRoad;

    while (getline(cin, inputLine)) {
        LineInfo line = make_pair(inputLine, lineCounter);
        checkLine(cars, roads, carsOnRoad, line);

        lineCounter++;
    }

    return 0;
}
