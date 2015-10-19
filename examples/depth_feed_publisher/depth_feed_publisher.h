#ifndef example_depth_feed_publisher_h
#define example_depth_feed_publisher_h

#include <stdexcept>
#include <boost/cstdint.hpp>
#include <boost/operators.hpp>
#include <boost/scoped_array.hpp>
#include <boost/shared_ptr.hpp>
#include <cstring>
#include <sstream>
#include <vector>

#include <Application/QuickFAST.h>
#include <Codecs/TemplateRegistry_fwd.h>
#include "example_order_book.h"
#include "book/types.h"
#include "depth_feed_connection.h"
#include "template_consumer.h"

namespace liquibook { namespace examples {

class DepthFeedPublisher : public ExampleOrderBook::TypedDepthListener,
                           public ExampleOrderBook::TypedTradeListener,
                           public ExampleOrderBook::TypedOrderListener,
                           public TemplateConsumer {
public:
  DepthFeedPublisher();
  void set_connection(DepthFeedConnection* connection);

  virtual void on_accept(const OrderPtr& order);
  virtual void on_reject(const OrderPtr& order, const char* reason);
  virtual void on_replace_reject(const OrderPtr& order, const char* reason);
  virtual void on_fill(const OrderPtr& order,
                   const OrderPtr& matched_order,
                   book::Quantity fill_qty,
                   book::Cost fill_cost);
  virtual void on_cancel(const OrderPtr& order);
  virtual void on_cancel_reject(const OrderPtr& order, const char* reason);
  virtual void on_replace(const OrderPtr& order,
                          const int32_t& size_delta,
                          book::Price new_price);

  virtual void on_trade(
      const book::OrderBook<OrderPtr>* order_book,
      book::Quantity qty,
      book::Cost cost);

  virtual void on_depth_change(
      const book::DepthOrderBook<OrderPtr>* order_book,
      const book::DepthOrderBook<OrderPtr>::DepthTracker* tracker);
private:
  DepthFeedConnection* connection_;

  // Build an trade message
  void build_trade_message(
      QuickFAST::Messages::FieldSet& message,
      const std::string& symbol,
      book::Quantity qty,
      book::Cost cost);

  // Build an incremental depth message
  void build_depth_message(
      QuickFAST::Messages::FieldSet& message,
      const std::string& symbol,
      const book::DepthOrderBook<OrderPtr>::DepthTracker* tracker,
      bool full_message);
  void build_depth_level(
      QuickFAST::Messages::SequencePtr& level_seq,
      const book::DepthLevel* level,
      int level_index);
  uint32_t time_stamp();
};

} } // End namespace
#endif
