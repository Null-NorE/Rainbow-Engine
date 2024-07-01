#pragma once
namespace RE {
    class fixpoint {
    public:
        fixpoint(int num);
        ~fixpoint() {};

    private:
        __int64 _num;
    };

    fixpoint::fixpoint(int n) :_num(n) {}
}