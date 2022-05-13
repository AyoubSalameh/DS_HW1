#ifndef TREE2NDATTEMPT_HIRINGCOMPANY_H
#define TREE2NDATTEMPT_HIRINGCOMPANY_H

#include "Tree.h"
#include "Employee.h"
#include "SalaryTreeKey.h"
#include "HighestEarner.h"


enum CompanyRes
{
    COMPANY_SUCCESS = 0,
    COMPANY_FAILURE = -1
};


///maybe change, so that when we add or remove, we send a pointer to the employee in general tree
class HiringCompany
{
private:
    int companyID;
    int value;
    int numOfEmployees;
    HighestEarner companyHighestEarner;
    Tree<int, Employee> companyIDTree;
    Tree<SalaryTreeKey, Employee> companySalaryTree;

public:

    HiringCompany();
    HiringCompany(int CompanyID, int value);
    ~HiringCompany() = default;
    HiringCompany(const HiringCompany& other);
    HiringCompany& operator=(const HiringCompany& other) = default;
    int operator()();

    int getCompanyID() const;
    int getCompanyValue() const;
    int getCompanyNumOfEmployees() const;
    HighestEarner getCompanyHighestEarner() const;
    ///maybe we need a get for each tree

    void increaseCompanyValue(int ValueIncrease);
    void increaseCompanyNumOfEmployees();
    void decreaseCompanyNumOfEmployees();
    void setCompanyNumOfEmployees(int num);     //cases where a company buys another
    void setCompanyValue(int val);
    void setCompanyHighestEarner(HighestEarner high);


    CompanyRes AddEmployee(int employeeID, int salary, int grade, Node<int, Employee> *IDTreeEmp,
                           Node<SalaryTreeKey, Employee> *SalTreeEmp);
    CompanyRes removeEmployee(int employeeID);

    //in this promote function, we can remove and add the employee again
    //it stays as O(logN)
    CompanyRes promoteEmployee(int employeeID, int salaryIncrease, int bumpGrade,
                               Node<int,Employee>* newIDTreeEmp,
                               Node<SalaryTreeKey, Employee>* newSalTreeEmp);

    ///i might not need the keys;
    CompanyRes fillArrayIDTreeData(int* keys, Employee* data);

    CompanyRes fillArrayReverseSalaryTreeKey(SalaryTreeKey* keys);

    CompanyRes GetNumMatchingInCompany(int minID, int maxID, int minSalary, int minGrade,
                                       int* TotalNumOfEmployees, int* NumOfEmployees);

    CompanyRes fillArrayKeyAndDataIDTree(int* keyArr, Employee* dataArr);
    CompanyRes fillArrayKeyAndDataSalaryTree(SalaryTreeKey* keyArr, Employee* dataArr);

    CompanyRes CreateIDTree(int* keyArr, Employee* dataArr, int size);
    CompanyRes CreateSalaryTree(SalaryTreeKey* keyArr, Employee* dataArr, int size);


};


#endif //TREE2NDATTEMPT_HIRINGCOMPANY_H
