//
// Created by Ayoub Salameh on 20/04/2022.
//

#include "Employee.h"

Employee::Employee() : id(0), salary(0), grade(-1), companyID(0),
                       employeeInIDTree(nullptr), employeeInSalaryTree(nullptr)
{}

Employee::Employee(int id, int salary, int grade, int companyID,
                   Node<int, Employee> *employeeInIDTree,
                   Node<SalaryTreeKey, Employee> *employeeInSalaryTree) :
                   id(id), salary(salary), grade(grade), companyID(companyID),
                   employeeInIDTree(employeeInIDTree),
                   employeeInSalaryTree(employeeInSalaryTree)
                   {}

int Employee::getID() const
{
    return this->id;
}

int Employee::getSalary() const
{
    return this->salary;
}

int Employee::getGrade() const
{
    return this->grade;
}

int Employee::getCompanyID() const
{
    return this->companyID;
}

Node<int, Employee>* Employee::getEmployeeInIDTree()
{
    return this->employeeInIDTree;
}

Node<SalaryTreeKey, Employee>* Employee::getEmployeeInSalaryTree()
{
    return this->employeeInSalaryTree;
}

void Employee::increaseSalary(int salaryIncrease)
{
    salary += salaryIncrease;
}

void Employee::increaseGrade(int bumpGrade)
{
    if(bumpGrade > 0)
        grade++;
    return;
}

void Employee::changeCompany(int newCompanyID)
{
    companyID = newCompanyID;
}

void Employee::setPointerToIDTree(Node<int, Employee> *empInIDTree)
{
    employeeInIDTree = empInIDTree;
}

void Employee::setPointerToSalaryTree(Node<SalaryTreeKey, Employee> *empInSalaryTree)
{
    employeeInSalaryTree = empInSalaryTree;
}

void Employee::setSalary(int newSalary)
{
    this->salary = newSalary;
}

void Employee::setGrade(int newGrade)
{
    this->grade = newGrade;
}

