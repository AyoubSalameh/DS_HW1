//
// Created by Ayoub Salameh on 20/04/2022.
//

#include "HighestEarner.h"

HighestEarner::HighestEarner(int id, int salary) : id(id), salary(salary)
{}

int HighestEarner::getID() const
{
    return this->id;
}

int HighestEarner::getSalary() const
{
    return this->salary;
}

void HighestEarner::setID(int newID)
{
    this->id = newID;
}

void HighestEarner::setSalary(int newSalary)
{
    this->salary = newSalary;
}

void HighestEarner::compareAndSetSalaries(int newID, int newSalary)
{
    if(newSalary < this->salary)
        return;
    if(newSalary > this->salary)
    {
        this->setSalary(newSalary);
        this->setID(newID);
        return;
    }
    if(newID < this->id)
    {
        this->setSalary(newSalary);
        this->setID(newID);
        return;
    }
    return;
}
