//
// Created by maxim on 18.10.2019.
//

#include <iostream>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "algorithms/TaneX.h"

using ::testing::ContainerEq, ::testing::Eq;
using namespace std;

std::string get_selfpath();

//TODO: wrong file => WARNING
TEST(TaneTester, first) {
    vector<string> datasets = {"CIPublicHighway50k.csv", "CIPublicHighway100k.csv", "CIPublicHighway200k.csv", "neighbors100k.csv"};
    //string path = "/home/maxim/Study/Metanome-coding/metanome_git/src/tests";
    auto path = fs::current_path().append("inputData").append(datasets[1]);
    //CSVParser csvParser(path + "/TestTane.csv");
    //cout << path;
    vector<long> results;
    try {
        Tane algoInstance(path);
        //Tane algoInstance(path + "/CIPublicHighway50k.csv");
        results.push_back(algoInstance.execute());
    }
    catch (std::runtime_error& e) {
        cout << "Exception raised in test: " << e.what() << endl;
        FAIL();
    }

    /*for (int i = 0; i < 10; i++) {
        Tane algoInstance(path + "/neighbors50k.csv");
        //Tane algoInstance(path + "/CIPublicHighway50k.csv");
        results.push_back(algoInstance.execute());
    }
    for (int i = 0; i < 10; i++) {
        Tane algoInstance(path + "/CIPublicHighway100k.csv");
        //Tane algoInstance(path + "/CIPublicHighway50k.csv");
        results.push_back(algoInstance.execute());
    }
    for (int i = 0; i < 10; i++) {
        Tane algoInstance(path + "/LegacyOPMeds.csv");
        //Tane algoInstance(path + "/CIPublicHighway50k.csv");
        results.push_back(algoInstance.execute());
    }
    for (int i = 0; i < 10; i++) {
        Tane algoInstance(path + "/LegacyIPMeds.csv");
        //Tane algoInstance(path + "/CIPublicHighway50k.csv");
        results.push_back(algoInstance.execute());
    }
    for (int i = 0; i < 10; i++) {
        Tane algoInstance(path + "/EpicVitals.csv");
        //Tane algoInstance(path + "/CIPublicHighway50k.csv");
        results.push_back(algoInstance.execute());
    }*/

    for (auto res : results)
        cout << res << " ";
    cout << endl;
    SUCCEED();
}