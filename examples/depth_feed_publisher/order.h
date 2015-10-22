#ifndef example_order_h
#define example_order_h
#include <iostream>
#include "book/order.h"

using namespace std;
namespace liquibook { namespace examples {

enum OrderState {
  os_new,
  os_accepted,
  os_complete,
  os_cancelled,
  os_rejected
};

class Order : public book::Order {
public:
  Order(bool buy,
        const double& price,          
        book::Quantity qty);
  Order(bool buy,
        const double& price,
        book::Quantity qty, const string& account, const string& order_id, const string& symbol);

  const OrderState& state() const;
  const book::Cost& filled_cost() const;
  virtual bool is_buy() const;
  virtual book::Quantity order_qty() const;
  virtual book::Quantity open_qty() const;
  virtual book::Price price() const;
  virtual string order_id() const;
  virtual string symbol() const;
  virtual string account() const;
  virtual const book::Quantity& filled_qty() const;


  void accept();
  void cancel();
  void replace(book::Quantity size_delta, book::Price new_price);
  virtual void fill(book::Quantity fill_qty, book::Cost fill_cost, book::FillId fill_id);

  static const uint8_t precision_;
private:
  string order_id_;
  string symbol_;
  string account_;
  OrderState state_;

  bool is_buy_;
  book::Price    price_;
  book::Quantity order_qty_;
  book::Quantity filled_qty_;
  book::Cost filled_cost_;
//  static uint32_t last_order_id_;
};

} }

#endif
