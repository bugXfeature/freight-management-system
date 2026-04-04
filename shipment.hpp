#pragma once
#include "freight.hpp"
#include <memory>
#include <string>
#include <sstream>
#include <iomanip>
#include <ctime>

class Shipment {
private:
    std::string shipmentId;
    std::shared_ptr<Freight> freight; // association — Shipment HAS-A Freight
    std::string date;

    std::string generateId() {
        // Simple ID: RFM + timestamp last 6 digits
        auto t = std::time(nullptr);
        std::string ts = std::to_string(t);
        return "RFM-" + ts.substr(ts.size() - 6);
    }

    std::string currentDate() {
        auto t = std::time(nullptr);
        auto tm = *std::localtime(&t);
        std::ostringstream oss;
        oss << std::put_time(&tm, "%d-%m-%Y");
        return oss.str();
    }

public:
    explicit Shipment(std::shared_ptr<Freight> f)
        : freight(f) {
        shipmentId = generateId();
        date = currentDate();
    }

    // Returns full quotation as a structured map (we'll serialize to JSON in main.cpp)
    std::string getShipmentId() const { return shipmentId; }
    std::string getDate()       const { return date; }

    double getBaseFreight()    const { return freight->baseFreight(); }
    double getHandlingCharge() const { return freight->handlingCharge(); }
    double getTotalCost()      const { return freight->totalCost(); }
    std::string getRouteRules()const { return freight->routeRules(); }
    std::string getFreightType()const{ return freight->freightType(); }

    std::shared_ptr<Freight> getFreight() const { return freight; }
};
