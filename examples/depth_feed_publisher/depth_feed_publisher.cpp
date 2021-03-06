#include <iomanip>
#include <fstream>
#include "depth_feed_publisher.h"
#include <Codecs/DataDestination.h>
#include <Codecs/XMLTemplateParser.h>
#include <Messages/FieldIdentity.h>
#include <Messages/FieldSet.h>
#include <Messages/FieldSequence.h>
#include <Messages/FieldString.h>
#include <Messages/FieldUInt8.h>
#include <Messages/FieldUInt32.h>
#include <Messages/Sequence.h>
#include <json/json.h>
#include <beanstalk.hpp>
using namespace Beanstalk;
using namespace Json;

namespace liquibook { namespace examples { 

using namespace QuickFAST::Messages;

DepthFeedPublisher::DepthFeedPublisher()
: connection_(NULL)
{
}

void
DepthFeedPublisher::set_connection(DepthFeedConnection* connection)
{
  connection_ = connection;
}

void
DepthFeedPublisher::on_accept(const OrderPtr& order)
{
  Json::Value data;
  data["event"]     = "on_accept";

  data["order_id"]  = order->order_id();
  data["symbol"]    = order->symbol();
  data["account"]   = order->account();
  data["is_buy"]    = order->is_buy();
  data["order_qty"] = order->order_qty();
  data["price"]     = order->price();
  data["state"]     = order->state();

  Client client("192.168.147.130", 11300);
  client.use("engine");
  client.put(data.toStyledString());

//  Json::Value data;
//  data["event"]         = "on_accept";
//  data["order_id"]      = order->order_id();
//  data["fill_qty"]      = order->order_id();
//  data["fill_cost"]     = fill_cost;
//
//  Client client("192.168.147.130", 11300);
//  client.use("engine");
//  client.put(data.toStyledString());
  std::cout << "on_accept " << order->order_id() <<" qty "<<order->order_qty() <<" open_qty "<<order->open_qty() << std::endl;
}
void
DepthFeedPublisher::on_reject(const OrderPtr& order, const char* reason)
{
  std::cout << "on_reject " << std::endl;
}
void
DepthFeedPublisher::on_replace_reject(const OrderPtr& order, const char* reason)
{
  std::cout << "on_replace_reject " << std::endl;
}
void
DepthFeedPublisher::on_fill(const OrderPtr& order,
                     const OrderPtr& matched_order,
                     book::Quantity fill_qty,
                     book::Cost fill_cost)
{
  Json::Value data;
  data["event"]            = "on_fill";
  data["order_id"]         = order->order_id();
  data["matched_order_id"] = matched_order->order_id();
  data["fill_qty"]         = fill_qty;
  data["fill_cost"]        = fill_cost;

  Client client("192.168.147.130", 11300);
  client.use("engine");
  client.put(data.toStyledString());

  std::cout << "on_fill.order......... " << order->order_id()<<" state "<< order->state()<<" qty "<< order->order_qty() <<" filled_qty "<<order->filled_qty() <<" open_qty "<<order->open_qty() <<std::endl;
  std::cout << "on_fill.matched_order. " << matched_order->order_id()<<" state "<< matched_order->state() <<" qty "<< matched_order->order_qty() <<" filled_qty "<<matched_order->filled_qty() <<" open_qty "<<matched_order->open_qty() <<std::endl;
}
void
DepthFeedPublisher::on_cancel(const OrderPtr& order)
{
  std::cout << "on_cancel " << std::endl;
  Json::Value data;
  data["event"]         = "on_cancel";
  data["order_id"]      = order->order_id();

  Client client("192.168.147.130", 11300);
  client.use("engine");
  client.put(data.toStyledString());
}
void
DepthFeedPublisher::on_cancel_reject(const OrderPtr& order, const char* reason)
{
  std::cout << "on_cancel_reject " << std::endl;
  Json::Value data;
  data["event"]         = "on_cancel";
  data["order_id"]      = order->order_id();

  Client client("192.168.147.130", 11300);
  client.use("engine");
  std::cout << "on_cancel_reject " << std::endl;
}
void
DepthFeedPublisher::on_replace(const OrderPtr& order,
                            const int32_t& size_delta,
                            book::Price new_price)
{
  std::cout << "on_replace " << std::endl;
  Json::Value data;
  data["event"]         = "on_replace";
  data["order_id"]      = order->order_id();

  Client client("192.168.147.130", 11300);
  client.use("engine");
  std::cout << "on_cancel_reject " << std::endl;
}

void
DepthFeedPublisher::on_trade(
    const book::OrderBook<OrderPtr>* order_book,
    book::Quantity qty,
    book::Cost cost)
{
  // Publish trade
  QuickFAST::Messages::FieldSet message(20);
  const ExampleOrderBook* exob = 
          dynamic_cast<const ExampleOrderBook*>(order_book);
  std::cout << "Got trade for " << exob->symbol() 
            << " qty " << qty
            << " cost " << cost << std::endl;
  build_trade_message(message, exob->symbol(), qty, cost);

  connection_->send_trade(message);
//  std::cout << "connection.on_trade..................." << std::endl;
//  std::cout << "connection.send_trade..................." << std::endl;
  Json::Value data;
  data["event"]   = "on_trade";
  data["symbol"]  = exob->symbol();
  data["qty"]     = qty;
  data["cost"]    = cost;

  Client client("192.168.147.130", 11300);
  client.use("engine");
  client.put(data.toStyledString());
}

void
DepthFeedPublisher::on_depth_change(
    const book::DepthOrderBook<OrderPtr>* order_book,
    const book::DepthOrderBook<OrderPtr>::DepthTracker* tracker)
{
  // Publish changed levels of order book
  QuickFAST::Messages::FieldSet message(20);
  const ExampleOrderBook* exob = 
          dynamic_cast<const ExampleOrderBook*>(order_book);
//  std::cout << "connection.on_depth_change..................." << std::endl;
  build_depth_message(message, exob->symbol(), tracker, false);
  if (!connection_->send_incr_update(exob->symbol(), message)) {
    // Publish all levels of order book
    QuickFAST::Messages::FieldSet full_message(20);
    build_depth_message(full_message, exob->symbol(), tracker, true);
    connection_->send_full_update(exob->symbol(), full_message);
//    std::cout << "connection.send_full_update..................." << std::endl;
  }
}
 
void
DepthFeedPublisher::build_trade_message(
    QuickFAST::Messages::FieldSet& message,
    const std::string& symbol,
    book::Quantity qty,
    book::Cost cost)
{
  message.addField(id_timestamp_, FieldUInt32::create(time_stamp()));
  message.addField(id_symbol_, FieldString::create(symbol));
  message.addField(id_qty_, FieldUInt32::create(qty));
  message.addField(id_cost_, FieldUInt32::create(cost));
}

void
DepthFeedPublisher::build_depth_message(
    QuickFAST::Messages::FieldSet& message,
    const std::string& symbol,
    const book::DepthOrderBook<OrderPtr>::DepthTracker* tracker,
    bool full_message)
{
//  std::cout << "build_depth_message..................." << full_message << std::endl;
  size_t bid_count(0), ask_count(0);

  message.addField(id_timestamp_, FieldUInt32::create(time_stamp()));
  message.addField(id_symbol_, FieldString::create(symbol));

  // Build the changed levels
  book::ChangeId last_published_change = tracker->last_published_change();
  
  // Build changed bids
  {
    SequencePtr bids(new Sequence(id_bids_length_, 1));
    int index = 0;
    const book::DepthLevel* bid = tracker->bids();
    // Create sequence of bids
    while (true) {
      if (full_message || bid->changed_since(last_published_change)) {
        build_depth_level(bids, bid, index);
        ++bid_count;
      }
      ++index;
      if (bid == tracker->last_bid_level()) {
        break;
      } else {
        ++bid;
      }
    }
    message.addField(id_bids_, FieldSequence::create(bids));
  }

  // Build changed asks
  {
    SequencePtr asks(new Sequence(id_asks_length_, 1));
    int index = 0;
    const book::DepthLevel* ask = tracker->asks();
    // Create sequence of asks
    while (true) {
      if (full_message || ask->changed_since(last_published_change)) {
        build_depth_level(asks, ask, index);
        ++ask_count;
      }
      ++index;
      if (ask == tracker->last_ask_level()) {
        break;
      } else {
        ++ask;
      }
    }
    message.addField(id_asks_, FieldSequence::create(asks));
  }
  std::cout << "Encoding " << (full_message ? "full" : "incr")
            << " depth message for symbol " << symbol 
            << " with " << bid_count << " bids, "
            << ask_count << " asks" << std::endl;
  Json::Value data;
  data["symbol"]  = symbol;
  data["bids"]    = (UInt64) bid_count;
  data["asks"]    = (UInt64) ask_count;
  data["full"]    = full_message;

  Client client("192.168.147.130", 11300);
  client.use("exchange.on_depth_change");
  client.put(data.toStyledString());
}

void
DepthFeedPublisher::build_depth_level(
    QuickFAST::Messages::SequencePtr& level_seq,
    const book::DepthLevel* level,
    int level_index)
{
  FieldSetPtr level_fields(new FieldSet(4));
  level_fields->addField(id_level_num_, FieldUInt8::create(level_index));
  level_fields->addField(id_order_count_, 
                         FieldUInt32::create(level->order_count()));
  level_fields->addField(id_price_,
                         FieldUInt32::create(level->price()));
  level_fields->addField(id_size_,
                         FieldUInt32::create(level->aggregate_qty()));
  level_seq->addEntry(level_fields);
}

uint32_t
DepthFeedPublisher::time_stamp()
{
  time_t now;
  time(&now);
  return now;
}

} } // End namespace
