#ifndef example_order_h
#define example_order_h
#include <iostream>
#include "book/order.h"

using namespace std;
namespace liquibook { namespace examples {

class Order : public book::Order {
public:
  Order(bool buy,
        const double& price,          
        book::Quantity qty);
  Order(bool buy,
        const double& price,
        book::Quantity qty, const string& account, const string& id);

  virtual bool is_buy() const;
  virtual book::Quantity order_qty() const;
  virtual book::Price price() const;
  virtual string account() const;
  virtual string id() const;

  static const uint8_t precision_;
private:
  bool is_buy_;
  double price_;
  string account_;
  string id_;
  book::Quantity qty_;
};

} }

#endif
