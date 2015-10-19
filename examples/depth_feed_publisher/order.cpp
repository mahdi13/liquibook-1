#include "order.h"

namespace liquibook { namespace examples {

const uint8_t Order::precision_(100);

Order::Order(bool buy, const double& price, book::Quantity qty, const std::string& account, const std::string& id)
: is_buy_(buy),
  price_(price),
  qty_(qty),
  account_(account),
  id_(id)
{
}

std::string
Order::id() const
{
  return id_;
}

std::string
Order::account() const
{
  return account_;
}

bool
Order::is_buy() const
{
  return is_buy_;
}

book::Quantity
Order::order_qty() const
{
  return qty_;
}

book::Price
Order::price() const
{
  return price_ * precision_;
}

} } // End namespace

