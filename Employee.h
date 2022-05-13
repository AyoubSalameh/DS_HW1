#ifndef TREE2NDATTEMPT_EMPLOYEE_H
#define TREE2NDATTEMPT_EMPLOYEE_H

#include "Tree.h"
#include "SalaryTreeKey.h"

class Employee
{
private:
    int id;
    int salary;
    int grade;
    int companyID;
    Node<int,Employee>* employeeInIDTree;
    Node<SalaryTreeKey, Employee>* employeeInSalaryTree;

public:
    Employee();
    Employee(int id, int salary, int grade, int companyID,
             Node<int,Employee>* employeeInIDTree = nullptr,
             Node<SalaryTreeKey, Employee>* employeeInSalaryTree = nullptr);
    Employee(const Employee& other) = default;
    ~Employee() = default;
    Employee& operator=(const Employee& other) = default;
    Employee& operator=(Employee& other) = default;


    int getID() const;
    int getSalary() const;
    int getGrade() const;
    int getCompanyID() const;

    ///these pointers are useless in the general employee trees, therefore will be NULL
    Node<int, Employee>* getEmployeeInIDTree();
    Node<SalaryTreeKey, Employee>* getEmployeeInSalaryTree();

    void setSalary(int newSalary);
    void setGrade(int newGrade);
    void increaseSalary(int salaryIncrease);
    void increaseGrade(int bumpGrade);
    void changeCompany(int newCompanyID);
    void setPointerToIDTree(Node<int,Employee>* empInIDTree);
    void setPointerToSalaryTree(Node<SalaryTreeKey, Employee>* empInSalaryTree);

};



#endif //TREE2NDATTEMPT_EMPLOYEE_H
