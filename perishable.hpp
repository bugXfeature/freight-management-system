#pragma once
#include "freight.hpp"

class Perishable : public Freight {
private:
    bool temperatureControlled;
    int expiryHours; // hours until expiry

public:
    Perishable(double weight, const std::string& origin, const std::string& dest,
               double distance, const std::string& sender, const std::string& receiver,
               bool tempControl, int expiry)
        : Freight(weight, origin, dest, distance, sender, receiver),
          temperatureControlled(tempControl), expiryHours(expiry) {}

    // Handling charge: 15% of base + INR 200 if temp controlled
    double handlingCharge() const override {
        double charge = baseFreight() * 0.15;
        if (temperatureControlled) charge += 200.0;
        return charge;
    }

    std::string routeRules() const override {
        std::string rules = "Priority routing required. ";
        if (temperatureControlled)
            rules += "Refrigerated wagons mandatory. ";
        if (expiryHours < 24)
            rules += "Express delivery — perishable expires within 24 hours. ";
        else
            rules += "Standard perishable handling applies. ";
        return rules;
    }

    std::string freightType() const override { return "Perishable"; }

    bool isTempControlled() const { return temperatureControlled; }
    int getExpiryHours()    const { return expiryHours; }
};
