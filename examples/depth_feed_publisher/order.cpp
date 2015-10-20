#include "order.h"

namespace liquibook { namespace examples {

const uint8_t Order::precision_(100);

Order::Order(bool buy, const double& price, book::Quantity qty, const std::string& account, const std::string& id)
: state_(os_new),
  is_buy_(buy),
  price_(price),
  order_qty_(qty),
  filled_qty_(0),
  filled_cost_(0),
  id_(id),
  account_(account)
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
  return order_qty_;
}

book::Price
Order::price() const
{
  return price_ * precision_;
}
void
Order::accept()
{
  if (os_new == state_) {
    state_ = os_accepted;
  }
}

void
Order::cancel()
{
  if (os_complete != state_) {
    state_ = os_cancelled;
  }
}

void
Order::replace(book::Quantity size_delta, book::Price new_price)
{
  if (os_accepted == state_) {
    order_qty_ += size_delta;
    price_ = new_price;
  }
}
book::Quantity
Order::open_qty() const
{
  // If not completely filled, calculate
  if (filled_qty_ < order_qty_) {
    return order_qty_ - filled_qty_;
    // Else prevent accidental overflow
  } else {
    return 0;
  }
}

void Order::fill(book::Quantity fill_qty, book::Cost fill_cost, book::FillId /*fill_id*/)
{
  filled_qty_ += fill_qty;
  filled_cost_ += fill_cost;
  if (!open_qty()) {
    state_ = os_complete;
  }
}
const OrderState& Order::state() const
{
  return state_;
}
const book::Quantity& Order::filled_qty() const
{
  return filled_qty_;
}

const book::Cost& Order::filled_cost() const
{
  return filled_cost_;
}
} } // End namespace

