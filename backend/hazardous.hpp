#pragma once
#include "freight.hpp"

class Hazardous : public Freight {
private:
    int hazardClass;    // 1–9 (UN hazard classification)
    bool requiresEscort;

public:
    Hazardous(double weight, const std::string& origin, const std::string& dest,
              double distance, const std::string& sender, const std::string& receiver,
              int hClass, bool escort)
        : Freight(weight, origin, dest, distance, sender, receiver),
          hazardClass(hClass), requiresEscort(escort) {
        if (hClass < 1 || hClass > 9)
            throw std::invalid_argument("Hazard class must be between 1 and 9.");
    }

    // Handling charge: 25% of base + INR 500 if escort required
    double handlingCharge() const override {
        double charge = baseFreight() * 0.25;
        if (requiresEscort) charge += 500.0;
        return charge;
    }

    std::string routeRules() const override {
        std::string rules = "Hazard Class " + std::to_string(hazardClass) + " — ";
        switch (hazardClass) {
            case 1: rules += "Explosives. Isolated wagon, no passenger trains nearby. "; break;
            case 2: rules += "Gases. Pressure-rated containers required. "; break;
            case 3: rules += "Flammable liquids. No open flame zones on route. "; break;
            case 4: rules += "Flammable solids. Dry storage mandatory. "; break;
            case 5: rules += "Oxidizers. Separate from flammables. "; break;
            case 6: rules += "Toxic/Infectious. Sealed containers, hazmat staff. "; break;
            case 7: rules += "Radioactive. Shielded wagon, restricted route. "; break;
            case 8: rules += "Corrosives. Acid-resistant lining required. "; break;
            case 9: rules += "Miscellaneous hazardous. Standard HAZCHEM protocol. "; break;
        }
        if (requiresEscort)
            rules += "Armed/safety escort mandated for full journey.";
        return rules;
    }

    std::string freightType() const override { return "Hazardous"; }

    int getHazardClass()    const { return hazardClass; }
    bool getRequiresEscort()const { return requiresEscort; }
};
