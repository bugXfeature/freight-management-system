#include <crow.h>
#include "freight.hpp"
#include "perishable.hpp"
#include "nonperishable.hpp"
#include "hazardous.hpp"
#include "shipment.hpp"
#include <memory>
#include <stdexcept>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <filesystem>

// Read a file from disk and return its contents as a string
std::string readFile(const std::string& path) {
    std::ifstream f(path);
    if (!f.is_open()) return "";
    return std::string(std::istreambuf_iterator<char>(f),
                       std::istreambuf_iterator<char>());
}

// Detect MIME type from file extension
std::string mimeType(const std::string& path) {
    auto ends = [&](const std::string& ext) {
        return path.size() >= ext.size() &&
               path.rfind(ext) == path.size() - ext.size();
    };
    if (ends(".html")) return "text/html";
    if (ends(".css"))  return "text/css";
    if (ends(".js"))   return "application/javascript";
    return "text/plain";
}
std::string fmt(double val) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << val;
    return oss.str();
}

struct CORSMiddleware {
    struct context {};

    void before_handle(crow::request& req, crow::response& res, context&) {
        if (req.method == crow::HTTPMethod::OPTIONS) {
            res.add_header("Access-Control-Allow-Origin", "*");
            res.add_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
            res.add_header("Access-Control-Allow-Headers", "Content-Type, Accept");
            res.add_header("Access-Control-Max-Age", "86400");
            res.code = 204;
            res.end();
        }
    }

    void after_handle(crow::request&, crow::response& res, context&) {
        res.add_header("Access-Control-Allow-Origin", "*");
        res.add_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
        res.add_header("Access-Control-Allow-Headers", "Content-Type, Accept");
    }
};

int main() {
    crow::App<CORSMiddleware> app;

    // ── Static frontend files ─────────────────────────────────────
    // The frontend folder is at ../../frontend relative to the binary
    // which lives in backend/build/
    const std::string FRONTEND = "/home/jesus/freight-system/frontend";

    CROW_ROUTE(app, "/")([ &FRONTEND]() {
        std::string html = readFile(FRONTEND + "/index.html");
        if (html.empty()) return crow::response(404, "index.html not found");
        crow::response resp(200, html);
        resp.add_header("Content-Type", "text/html");
        return resp;
    });

    CROW_ROUTE(app, "/style.css")([&FRONTEND]() {
        std::string css = readFile(FRONTEND + "/style.css");
        if (css.empty()) return crow::response(404, "style.css not found");
        crow::response resp(200, css);
        resp.add_header("Content-Type", "text/css");
        return resp;
    });

    CROW_ROUTE(app, "/app.js")([&FRONTEND]() {
        std::string js = readFile(FRONTEND + "/app.js");
        if (js.empty()) return crow::response(404, "app.js not found");
        crow::response resp(200, js);
        resp.add_header("Content-Type", "application/javascript");
        return resp;
    });

    // ── API routes ────────────────────────────────────────────────
    CROW_ROUTE(app, "/api/health")([]() {
        crow::json::wvalue res;
        res["status"] = "ok";
        res["service"] = "Railway Freight Management System";
        return crow::response(200, res);
    });

    CROW_ROUTE(app, "/api/quotation").methods(crow::HTTPMethod::POST)(
    [](const crow::request& req) {
        crow::response resp;
        resp.add_header("Content-Type", "application/json");

        try {
            auto body = crow::json::load(req.body);
            if (!body) {
                resp.code = 400;
                crow::json::wvalue err;
                err["error"] = "Invalid JSON body";
                resp.body = err.dump();
                return resp;
            }

            std::string type     = body["type"].s();
            double weight        = body["weight"].d();
            std::string origin   = body["origin"].s();
            std::string dest     = body["dest"].s();
            double distance      = body["distance"].d();
            std::string sender   = body["sender"].s();
            std::string receiver = body["receiver"].s();

            std::shared_ptr<Freight> freight;

            if (type == "perishable") {
                bool tempControl = body["tempControl"].b();
                int expiry       = (int)body["expiryHours"].i();
                freight = std::make_shared<Perishable>(
                    weight, origin, dest, distance, sender, receiver,
                    tempControl, expiry
                );
            }
            else if (type == "nonperishable") {
                std::string packaging = body["packaging"].s();
                bool fragile          = body["fragile"].b();
                freight = std::make_shared<NonPerishable>(
                    weight, origin, dest, distance, sender, receiver,
                    packaging, fragile
                );
            }
            else if (type == "hazardous") {
                int hazardClass  = (int)body["hazardClass"].i();
                bool escort      = body["escort"].b();
                freight = std::make_shared<Hazardous>(
                    weight, origin, dest, distance, sender, receiver,
                    hazardClass, escort
                );
            }
            else {
                resp.code = 400;
                crow::json::wvalue err;
                err["error"] = "Unknown freight type. Use: perishable, nonperishable, hazardous";
                resp.body = err.dump();
                return resp;
            }

            Shipment shipment(freight);

            crow::json::wvalue res;
            res["shipmentId"]     = shipment.getShipmentId();
            res["date"]           = shipment.getDate();
            res["freightType"]    = shipment.getFreightType();
            res["sender"]         = freight->getSender();
            res["receiver"]       = freight->getReceiver();
            res["origin"]         = freight->getOrigin();
            res["destination"]    = freight->getDest();
            res["weightKg"]       = fmt(freight->getWeight());
            res["distanceKm"]     = fmt(freight->getDistance());
            res["baseFreight"]    = fmt(shipment.getBaseFreight());
            res["handlingCharge"] = fmt(shipment.getHandlingCharge());
            res["totalCost"]      = fmt(shipment.getTotalCost());
            res["routeRules"]     = shipment.getRouteRules();

            resp.code = 200;
            resp.body = res.dump();
            return resp;

        } catch (const std::exception& e) {
            resp.code = 500;
            crow::json::wvalue err;
            err["error"] = std::string(e.what());
            resp.body = err.dump();
            return resp;
        }
    });

    app.port(8080).multithreaded().run();
    return 0;
}
