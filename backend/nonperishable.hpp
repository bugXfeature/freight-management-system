#pragma once
#include "freight.hpp"

class NonPerishable : public Freight {
private:
    std::string packagingType; // "crate", "pallet", "loose"
    bool isFragile;

public:
    NonPerishable(double weight, const std::string& origin, const std::string& dest,
                  double distance, const std::string& sender, const std::string& receiver,
                  const std::string& packaging, bool fragile)
        : Freight(weight, origin, dest, distance, sender, receiver),
          packagingType(packaging), isFragile(fragile) {}

    // Handling charge: 8% of base + INR 150 if fragile
    double handlingCharge() const override {
        double charge = baseFreight() * 0.08;
        if (isFragile) charge += 150.0;
        return charge;
    }

    std::string routeRules() const override {
        std::string rules = "Standard routing applies. ";
        if (isFragile)
            rules += "Fragile — no stacking, cushioned wagon required. ";
        if (packagingType == "loose")
            rules += "Loose cargo — secure bundling before loading. ";
        else
            rules += "Packaging type: " + packagingType + ". ";
        return rules;
    }

    std::string freightType() const override { return "Non-Perishable"; }

    std::string getPackagingType() const { return packagingType; }
    bool getIsFragile()            const { return isFragile; }
};
