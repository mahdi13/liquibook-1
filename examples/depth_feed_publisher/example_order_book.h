#ifndef example_order_book_h
#define example_order_book_h

#include "order.h"
#include "book/depth_order_book.h"
#include <boost/shared_ptr.hpp>

namespace liquibook { namespace examples {

typedef boost::shared_ptr<Order> OrderPtr;

class ExampleOrderBook : public book::DepthOrderBook<OrderPtr> {
public:
  ExampleOrderBook(const std::string& symbol);
  const std::string& symbol() const;
  typedef book::Callback<OrderPtr> SimpleCallback;
  typedef uint32_t FillId;

  ExampleOrderBook();

  // Override callback handling to update SimpleOrder state
  virtual void perform_callback(SimpleCallback& cb);

private:
  std::string symbol_;
  FillId fill_id_;
};


//ExampleOrderBook<book::DepthOrderBook<OrderPtr>>::ExampleOrderBook()
//    : fill_id_(0)
//{
//}

inline void
ExampleOrderBook::perform_callback(SimpleCallback& cb)
{
  book::DepthOrderBook<OrderPtr>::perform_callback(cb);
  switch(cb.type) {
    case SimpleCallback::cb_order_accept:
//      cb.order->accept();
      break;
    case SimpleCallback::cb_order_fill: {
      // Increment fill ID once
      ++fill_id_;
      // Update the orders
      std::cout << "cb_order_fill " << " id " << cb.order->order_id() << " matched_id " << cb.matched_order->order_id() <<std::endl;
      std::cout << "cb_order_fill " << " cb.fill_qty " << cb.fill_qty << " fill_price " << cb.fill_price <<std::endl;
      book::Cost fill_cost = cb.fill_qty * cb.fill_price;
      cb.matched_order->fill(cb.fill_qty, fill_cost, fill_id_);
      cb.order->fill(cb.fill_qty, fill_cost, fill_id_);
      std::cout << "cb_order_fill.order................. " << cb.order->order_id()<<" state "<< cb.order->state()<<" qty "<< cb.order->order_qty() <<" filled_qty "<<cb.order->filled_qty() <<" open_qty "<<cb.order->open_qty() <<std::endl;
      std::cout << "cb_order_fill.matched_order......... " << cb.matched_order->order_id()<<" state "<< cb.matched_order->state()<<" qty "<< cb.matched_order->order_qty() <<" filled_qty "<<cb.matched_order->filled_qty() <<" open_qty "<<cb.matched_order->open_qty() <<std::endl;
      break;
    }
    case SimpleCallback::cb_order_cancel:
      cb.order->cancel();
      break;
    case SimpleCallback::cb_order_replace:
      // Modify the order itself
      cb.order->replace(cb.repl_size_delta, cb.repl_new_price);
      break;
    default:
      // Nothing
      break;
  }
}
} } // End namespace

#endif
