#pragma once

namespace waba {

    enum contract_type : uint8_t {
        mutual_credit_system = 1
    };

    typedef struct contract_setting {

        uint64_t key;
        std::string value;

        uint64_t primary_key() const { return key; }

    } contract_setting;

    typedef eosio::multi_index<N(contract_settings), contract_setting> contract_settings_table;

    class token_contract : public eosio::contract {
    public:
        token_contract(account_name self) : contract(self) {}

        virtual void create(account_name issuer,
                            eosio::symbol_type symbol,
                            std::vector<contract_setting> contract_settings) const;


    };

}

