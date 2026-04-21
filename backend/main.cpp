#include "freight.hpp"
#include "perishable.hpp"
#include "nonperishable.hpp"
#include "hazardous.hpp"
#include "shipment.hpp"

#include <memory>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>

// ── Helpers ────────────────────────────────────────────────────

static std::string fmt(double val) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << val;
    return oss.str();
}

static std::string escapeJson(const std::string& s) {
    std::string result;
    for (char c : s) {
        switch (c) {
            case '"':  result += "\\\""; break;
            case '\\': result += "\\\\"; break;
            case '\n': result += "\\n";  break;
            case '\r': result += "\\r";  break;
            case '\t': result += "\\t";  break;
            default:   result += c;      break;
        }
    }
    return result;
}

static void printError(const std::string& msg) {
    std::cout << "{\"error\":\"" << escapeJson(msg) << "\"}" << std::endl;
}

// ── Main entry ─────────────────────────────────────────────────

int main(int argc, char* argv[]) {
    try {
        if (argc < 8) {
            printError("Usage: freight_processor <type> <weight> <origin> <dest> "
                       "<distance> <sender> <receiver> [type-specific args]");
            return 1;
        }

        std::string type = argv[1];
        double weight    = std::stod(argv[2]);
        std::string origin   = argv[3];
        std::string dest     = argv[4];
        double distance  = std::stod(argv[5]);
        std::string sender   = argv[6];
        std::string receiver = argv[7];

        std::shared_ptr<Freight> freight;

        if (type == "perishable") {
            if (argc < 10) { printError("Perishable requires: tempControl(0/1) expiryHours"); return 1; }
            bool tempControl = std::string(argv[8]) == "1";
            int expiryHours  = std::stoi(argv[9]);
            freight = std::make_shared<Perishable>(
                weight, origin, dest, distance, sender, receiver,
                tempControl, expiryHours
            );
        }
        else if (type == "nonperishable") {
            if (argc < 10) { printError("NonPerishable requires: packaging fragile(0/1)"); return 1; }
            std::string packaging = argv[8];
            bool fragile = std::string(argv[9]) == "1";
            freight = std::make_shared<NonPerishable>(
                weight, origin, dest, distance, sender, receiver,
                packaging, fragile
            );
        }
        else if (type == "hazardous") {
            if (argc < 10) { printError("Hazardous requires: hazardClass escort(0/1)"); return 1; }
            int hazardClass = std::stoi(argv[8]);
            bool escort     = std::string(argv[9]) == "1";
            freight = std::make_shared<Hazardous>(
                weight, origin, dest, distance, sender, receiver,
                hazardClass, escort
            );
        }
        else {
            printError("Unknown freight type. Use: perishable, nonperishable, hazardous");
            return 1;
        }

        Shipment shipment(freight);

        // Output structured JSON to stdout
        std::cout << "{"
            << "\"shipmentId\":\""     << escapeJson(shipment.getShipmentId())    << "\","
            << "\"date\":\""           << escapeJson(shipment.getDate())          << "\","
            << "\"freightType\":\""    << escapeJson(shipment.getFreightType())   << "\","
            << "\"sender\":\""         << escapeJson(freight->getSender())        << "\","
            << "\"receiver\":\""       << escapeJson(freight->getReceiver())      << "\","
            << "\"origin\":\""         << escapeJson(freight->getOrigin())        << "\","
            << "\"destination\":\""    << escapeJson(freight->getDest())          << "\","
            << "\"weightKg\":\""       << fmt(freight->getWeight())              << "\","
            << "\"distanceKm\":\""     << fmt(freight->getDistance())            << "\","
            << "\"baseFreight\":\""    << fmt(shipment.getBaseFreight())         << "\","
            << "\"handlingCharge\":\"" << fmt(shipment.getHandlingCharge())      << "\","
            << "\"totalCost\":\""      << fmt(shipment.getTotalCost())           << "\","
            << "\"routeRules\":\""     << escapeJson(shipment.getRouteRules())   << "\""
            << "}" << std::endl;

        return 0;

    } catch (const std::exception& e) {
        printError(e.what());
        return 1;
    }
}
