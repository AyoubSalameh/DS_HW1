#ifndef TREE2NDATTEMPT_SALARYTREEKEY_H
#define TREE2NDATTEMPT_SALARYTREEKEY_H

///because this is a key, we cant change it, hence there is no need to set functions
class SalaryTreeKey
{
private:
    int id;
    int salary;

public:
    SalaryTreeKey();
    SalaryTreeKey(int id, int salary);
    SalaryTreeKey(const SalaryTreeKey& other) = default;
    ~SalaryTreeKey() = default;
    SalaryTreeKey& operator=(const SalaryTreeKey& other) = default;

    friend bool operator<(const SalaryTreeKey& left, const SalaryTreeKey& right);
    friend bool operator>(const SalaryTreeKey& left, const SalaryTreeKey& right);
    friend bool operator==(const SalaryTreeKey& left, const SalaryTreeKey& right);

    int getSalary() const;
    int getID() const;
};


#endif //TREE2NDATTEMPT_SALARYTREEKEY_H
