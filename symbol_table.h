#include<iostream>
#include <unordered_map>
#include <string>


enum kind
{
    static_k, field_k, var_k, arg_k, NONE
};
const std::string kind_str[] {"static","field","local","argument","invalid"};

struct symbol
{
    std::string symbol_name;
    std::string type;
    kind ki;
    int num;
};



class symbol_table
{
    std::unordered_map<std::string,symbol> symbols;
    std::unordered_map<kind,int> kind_count;
    char table_type;
public:
    symbol_table(char type)
    {
        if(type == 'c')
        {
            kind_count[kind::static_k] = 0;
            kind_count[kind::field_k] = 0;
            table_type = type;
        }
        else if(type == 's')
        {
            kind_count[kind::arg_k] = 0;
            kind_count[kind::var_k] = 0;
            table_type = type;
        }
        else
        {
            std::cerr << "Wrong table type" << std::endl;
            exit(0);
        }
    }

    void start_subroutine()
    {
        if(table_type == 's')
        {
            kind_count[kind::arg_k] = 0;
            kind_count[kind::var_k] = 0;
            symbols.clear();
        }
        else if(table_type == 'c')
        {
            std::cout << "can't restart a class symbol table" << std::endl;
        }
    }

    void define(std::string n,std::string t,kind k) // n = name, t = type, k = kind
    {
        if(table_type == 'c')
        {
            if(k == kind::static_k)
            {
                symbols.insert({n,symbol{n,t,k,kind_count[k]}});
                kind_count[k]+=1;
            }
            else if(k == kind::field_k)
            {
                symbols.insert({n,symbol{n,t,k,kind_count[k]}});
                kind_count[k]+=1;
            }
            else
            {
                std::cerr << "Wrong table type" << std::endl;
                exit(0);
            }
        }


        else if(table_type == 's')
        {
            if(k == kind::var_k)
            {
                symbols.insert({n,symbol{n,t,k,kind_count[k]}});
                kind_count[k]+=1;
            }
            else if(k == kind::arg_k)
            {
                symbols.insert({n,symbol{n,t,k,kind_count[k]}});
                kind_count[k] += 1;
            }
            else
            {
                std::cerr << "Wrong kind type" << std::endl;
                exit(0);
            }
        }
    }


    int var_count(kind k)
    {
        if(table_type == 'c')
        {
            if(k == kind::field_k && k == kind::static_k)
            {
                return kind_count[k];
            }
            else
            {
                std::cerr << "wrong kind " << std::endl;
                exit(0);
            }
        }
        else 
        {
            if(k == kind::arg_k && k == kind::var_k)
            {
                return kind_count[k];
            }
            else
            {
                std::cerr << "Wrong kind " << std::endl;
                exit(0);
            }
        }
    }

    kind kind_of(std::string str)
    {
        return symbols[str].ki;
    }


    std::string type_of(std::string str)
    {
        return symbols[str].type;
    }

    int index_of(std::string str)
    {
        return symbols[str].num;
    }

    bool symbol_exists_of(std::string str)
    {
        std::unordered_map<std::string,symbol>::const_iterator got = symbols.find(str);
        if(got == symbols.end())
        {
            return false;
        }
        else
        {
            return true;
        }

    }

    

};
