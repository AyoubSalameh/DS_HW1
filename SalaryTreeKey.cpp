//
// Created by Ayoub Salameh on 20/04/2022.
//

#include "SalaryTreeKey.h"

SalaryTreeKey::SalaryTreeKey() : id(0), salary(0)
{}

SalaryTreeKey::SalaryTreeKey(int id, int salary) :  id(id), salary(salary)
{}

int SalaryTreeKey::getSalary() const
{
    return this->salary;
}

int SalaryTreeKey::getID() const
{
    return this->id;
}

bool operator<(const SalaryTreeKey &left, const SalaryTreeKey &right)
{
    if(left.salary != right.salary)
        return left.salary < right.salary;
    return right.id < left.id;
}

bool operator==(const SalaryTreeKey &left, const SalaryTreeKey &right)
{
    return left.id == right.id;
}

bool operator>(const SalaryTreeKey &left, const SalaryTreeKey &right) {
    return right<left;
}

