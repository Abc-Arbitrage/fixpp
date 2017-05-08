/* message_type.h
   Nicolas Fauvet, 08 may 2017
   
   A typed list of FIX message types
*/

#pragma once

#include <fixpp/dsl/details/chars.h>

namespace Fixpp
{
    namespace MessageType
    {
        using Heartbeat = Chars<'0'>;
        using TestRequest = Chars<'1'>;
        using ResendRequest = Chars<'2'>;
        using Reject = Chars<'3'>;
        using SequenceReset = Chars<'4'>;
        using Logout = Chars<'5'>;
        using IndicationofInterest = Chars<'6'>;
        using Advertisement = Chars<'7'>;
        using ExecutionReport = Chars<'8'>;
        using OrderCancelReject = Chars<'9'>;
        using QuoteStatusRequest = Chars<'a'>;
        using Logon = Chars<'A'>;
        using QuoteAcknowledgement = Chars<'b'>;
        using News = Chars<'B'>;
        using SecurityDefinitionRequest = Chars<'c'>;
        using Email = Chars<'C'>;
        using SecurityDefinition = Chars<'d'>;
        using OrderSingle = Chars<'D'>;
        using SecurityStatusRequest = Chars<'e'>;
        using OrderList = Chars<'E'>;
        using SecurityStatus = Chars<'f'>;
        using OrderCancelRequest = Chars<'F'>;
        using TradingSessionStatusRequest = Chars<'g'>;
        using OrderCancelReplaceRequest = Chars<'G'>;
        using TradingSessionStatus = Chars<'h'>;
        using OrderStatusRequest = Chars<'H'>;
        using MassQuote = Chars<'i'>;
        using BusinessMessageReject = Chars<'j'>;
        using Allocation = Chars<'J'>;
        using BidRequest = Chars<'k'>;
        using ListCancelRequest = Chars<'K'>;
        using BidResponse = Chars<'l'>;
        using ListExecute = Chars<'L'>;
        using ListStrikePrice = Chars<'m'>;
        using ListStatusRequest = Chars<'M'>;
        using ListStatus = Chars<'N'>;
        using AllocationAck = Chars<'P'>;
        using DontKnowTrade = Chars<'Q'>;
        using QuoteRequest = Chars<'R'>;
        using Quote = Chars<'S'>;
        using SettlementInstructions = Chars<'T'>;
        using MarketDataRequest = Chars<'V'>;
        using MarketDataSnapshotFullRefresh = Chars<'W'>;
        using MarketDataIncrementalRefresh = Chars<'X'>;
        using MarketDataRequestReject = Chars<'Y'>;
        using QuoteCancel = Chars<'Z'>;
        using QuoteRequestReject = Chars<'A', 'G'>;
        using QuoteResponse = Chars<'A', 'J'>;
        using UserRequest = Chars<'B', 'E'>;
        using UserResponse = Chars<'B', 'F'>;
    }

} // namespace Fixpp
