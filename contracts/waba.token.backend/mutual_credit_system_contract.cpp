#include <vector>
#include "mutual_credit_system_contract.hpp"

namespace waba {

    using std::vector;

    void mutual_credit_system_contract::validate_create(account_name issuer,
                                                        eosio::symbol_type symbol,
                                                        vector<setting> contract_settings) const {

        // TODO validate individual values
        for (setting &contract_setting : contract_settings) {
            bool found = false;
            for (const auto key : all_settings) {
                if (contract_setting.key == key) {
                    found = true;
                    break;
                }
            }

            eosio_assert(found, "invalid contract setting key");
        }
    }

    void mutual_credit_system_contract::initialize(account_name issuer,
                                                   eosio::symbol_type symbol,
                                                   vector<setting> contract_settings) const {
        // default impl is good enough for us
    }

    void mutual_credit_system_contract::issue(account_name from, account_name to, asset quantity, std::vector<setting> settings) const {
        symbol_type symbol = quantity.symbol;
        accounts_table issuer_accounts(_self, from);
        issuer_accounts.modify(issuer_accounts.get(symbol), 0, [&](auto &to_update) {
            to_update.overdraft = (asset(to_update.overdraft, symbol) + quantity).amount;
        });
    }

}/// namespace eosio
