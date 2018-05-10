/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */

#include "waba.token.backend.hpp"

namespace waba {

    using eosio::print;

    void token::create(account_name owner,
                       symbol_type symbol,
                       contract_type type,
                       vector<setting> contract_settings) {
        require_auth(_self);

        eosio_assert(symbol.is_valid(), "invalid symbol name");

        token_settings_table token_settings_table(_self, symbol.name());
        auto existing = token_settings_table.find(symbol.name());
        eosio_assert(existing == token_settings_table.end(), "token with symbol already exists");

        const token_contract &token_contract = this->get_contract(type);

        token_contract.validate_create(owner, symbol, contract_settings);

        token_settings_table.emplace(_self, [&](token_settings &token_settings) {
            token_settings.supply.symbol = symbol;
            token_settings.owner = owner;
            token_settings.contract_type = type;
            token_settings.contract_settings = contract_settings;
        });

        // initialize the owner account
        accounts_table issuer_accounts(_self, owner);
        issuer_accounts.emplace(owner, [&](auto &issuer) {
            issuer.balance = asset(0, symbol);
            issuer.issue_limit = asset::max_amount;
        });

        token_contract.initialize(owner, symbol, contract_settings);

    }


    void token::issue(account_name to, asset quantity, string memo) {
        print("issue");

        eosio_assert(quantity.is_valid(), "invalid quantity");
        eosio_assert(quantity.amount > 0, "must issue positive quantity");

        auto symbol = quantity.symbol.name();
        token_settings_table token_settings_table(_self, symbol);
        const token_settings &token_settings = token_settings_table.get(symbol);

        accounts_table issuer_accounts(_self, current_sender());
        const account& issuer = issuer_accounts.get(symbol);
        asset issued = asset(issuer.issued, symbol);
        asset limit = asset(issuer.issue_limit, symbol);

        eosio_assert( !issuer.frozen, "issuer is frozen" );
        eosio_assert(issued + quantity > limit, "specified quantity exceeds your issue limit");

        token_settings_table.modify(token_settings, 0, [&](auto &token_settings) {
            token_settings.supply += quantity;
        });

        const token_contract &token_contract = this->get_contract(token_settings.contract_type);

        issuer_accounts.modify(issuer, 0, [&](auto &issuer) {
            issuer.issued = (issued + quantity).amount;
            issuer.balance += quantity;
        });

        if (to != current_sender()) {
            SEND_INLINE_ACTION(*this, transfer, { current_sender(), N(active) },
                               { current_sender(), to, quantity, memo });
        }

        token_contract.issue(to, quantity);

    }

    void token::setissuelimit(account_name to, asset limit, string memo) {
        print("setissuelimit");

        auto sym = limit.symbol.name();
        token_settings_table token_settings_table(_self, sym);
        const auto &token_settings = token_settings_table.get(sym);

        // TODO: can only the managers allow issuing?
        require_auth(token_settings.owner);

        accounts_table receiver_accounts(_self, to);
        const account &receiver = receiver_accounts.get(limit.symbol.name());

        eosio_assert(receiver.issued > limit.amount, "already issued more than the new limit");

        receiver_accounts.modify(receiver, 0, [&](account &updatable_receiver) {
            updatable_receiver.issue_limit = limit.amount;
        });

    }

    void token::transfer(account_name from,
                         account_name to,
                         asset quantity,
                         string       /*memo*/) {
        print("transfer");
        require_auth(from);
        auto sym = quantity.symbol.name();
        token_settings_table token_settings_table(_self, sym);
        const auto &token_settings = token_settings_table.get(sym);

        require_recipient(from);
        require_recipient(to);

        eosio_assert(quantity.is_valid(), "invalid quantity");
        eosio_assert(quantity.amount > 0, "must transfer positive quantity");

        sub_balance(from, quantity, token_settings);
        // TODO: in the original token code, the ram payer is the sender,
        // we are changing that to the token owner account here
        add_balance(to, quantity, token_settings, token_settings.owner, from);
    }

    void token::sub_balance(account_name owner, asset value, const token_settings &st) {
        accounts_table from_acnts(_self, owner);

        const account &from = from_acnts.get(value.symbol.name());
        eosio_assert(from.balance.amount >= value.amount, "overdrawn balance");

        eosio_assert( !from.frozen, "sender is frozen" );

        if (!has_auth(owner)) {
            eosio_assert(false, "insufficient authority");
        }

        from_acnts.modify(from, owner, [&](auto &a) {
            a.balance -= value;
        });
    }

    void token::add_balance(account_name owner, asset value, const token_settings &st, account_name ram_payer, account_name referrer) {
        accounts_table to_acnts(_self, owner);
        auto to = to_acnts.find(value.symbol.name());
        if (to == to_acnts.end()) {
            to_acnts.emplace(ram_payer, [&](auto &a) {
                a.balance = value;
                a.referrer = referrer;
            });
        } else {
            to_acnts.modify(to, 0, [&](auto &a) {
                a.balance += value;
            });
        }
    }

    const token_contract& token::get_contract(contract_type type) {

        switch (type) {
            case mutual_credit_system:
                return mutual_credit_system_contract(_self);
            default:
                printf("unsupported contract type: %d", type);
                eosio_assert(false, "unsupported contract type");
                break;

        }

    }

} /// namespace eosio

EOSIO_ABI(waba::token, (create)(issue)(setissuelimit)(transfer))
