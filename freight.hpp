#pragma once
#include <string>
#include <stdexcept>

// Base rate per kg per km in INR
constexpr double BASE_RATE = 0.85;

class Freight {
protected:
    double weightKg;
    std::string originStation;
    std::string destStation;
    double distanceKm;
    std::string senderName;
    std::string receiverName;

public:
    Freight(double weight, const std::string& origin, const std::string& dest,
            double distance, const std::string& sender, const std::string& receiver)
        : weightKg(weight), originStation(origin), destStation(dest),
          distanceKm(distance), senderName(sender), receiverName(receiver) {
        if (weight <= 0 || distance <= 0)
            throw std::invalid_argument("Weight and distance must be positive.");
    }

    virtual ~Freight() = default;

    // Base freight cost: weight x distance x base_rate
    double baseFreight() const {
        return weightKg * distanceKm * BASE_RATE;
    }

    // Pure virtual — must be overridden by each derived class
    virtual double handlingCharge() const = 0;
    virtual std::string routeRules() const = 0;
    virtual std::string freightType() const = 0;

    double totalCost() const {
        return baseFreight() + handlingCharge();
    }

    // Getters
    double getWeight()       const { return weightKg; }
    double getDistance()     const { return distanceKm; }
    std::string getOrigin()  const { return originStation; }
    std::string getDest()    const { return destStation; }
    std::string getSender()  const { return senderName; }
    std::string getReceiver()const { return receiverName; }
};
