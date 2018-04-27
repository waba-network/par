/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */

#include "waba.token.backend.hpp"

namespace waba {

    using eosio::print;

    void token::create(account_name issuer,
                       symbol_type symbol,
                       contract_type type,
                       vector<contract_setting> contract_settings) {
        require_auth(_self);

        eosio_assert(symbol.is_valid(), "invalid symbol name");

        token_settings_table token_settings_table(_self, symbol.name());
        auto existing = token_settings_table.find(symbol.name());
        eosio_assert(existing == token_settings_table.end(), "token with symbol already exists");

        token_settings_table.emplace(_self, [&](token_settings &token_settings) {
            token_settings.supply.symbol = symbol;
            token_settings.issuer = issuer;
            token_settings.contract_type = type;
        });

        const token_contract &token_contract = this->get_contract(type);

        token_contract.create(issuer, symbol, contract_settings);

    }


    void token::issue(account_name to, asset quantity, string memo) {
        print("issue");
        auto sym = quantity.symbol.name();
        token_settings_table token_settings_table(_self, sym);
        const auto &token_settings = token_settings_table.get(sym);

        require_auth(token_settings.issuer);
        eosio_assert(quantity.is_valid(), "invalid quantity");
        eosio_assert(quantity.amount > 0, "must issue positive quantity");

        token_settings_table.modify(token_settings, 0, [&](auto &token_settings) {
            token_settings.supply += quantity;
        });

        add_balance(token_settings.issuer, quantity, token_settings, token_settings.issuer);

        if (to != token_settings.issuer) {
            SEND_INLINE_ACTION(*this, transfer, { token_settings.issuer, N(active) },
                               { token_settings.issuer, to, quantity, memo });
        }
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
        add_balance(to, quantity, token_settings, from);
    }

    void token::sub_balance(account_name owner, asset value, const token_settings &st) {
        accounts from_acnts(_self, owner);

        const auto &from = from_acnts.get(value.symbol.name());
        eosio_assert(from.balance.amount >= value.amount, "overdrawn balance");

        if (!has_auth(owner)) {
            eosio_assert(false, "insufficient authority");
        }

        from_acnts.modify(from, owner, [&](auto &a) {
            a.balance -= value;
        });
    }

    void token::add_balance(account_name owner, asset value, const token_settings &st, account_name ram_payer) {
        accounts to_acnts(_self, owner);
        auto to = to_acnts.find(value.symbol.name());
        if (to == to_acnts.end()) {
            to_acnts.emplace(ram_payer, [&](auto &a) {
                a.balance = value;
            });
        } else {
            to_acnts.modify(to, 0, [&](auto &a) {
                a.balance += value;
            });
        }
    }

    token_contract token::get_contract(contract_type type) {

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

EOSIO_ABI(waba::token, (create)(issue)(transfer))
