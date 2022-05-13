#ifndef TREE2NDATTEMPT_EMPLOYEEMANAGER_H
#define TREE2NDATTEMPT_EMPLOYEEMANAGER_H

#include "Tree.h"
#include "SalaryTreeKey.h"
#include "HighestEarner.h"
#include "Employee.h"
#include "HiringCompany.h"

#include "library1.h"



class EmployeeManager
{
private:
    Tree<int, int> CompanyTree;                             ///total companies
    Tree<int, HiringCompany> HiringCompaniesTree;           ///companies that are hiring
    Tree<int, Employee> EmployeeIDTree;                     ///all employees, by ID
    Tree<SalaryTreeKey, Employee> EmployeeSalaryTree;       ///all employees, by salary and ID
    int numOfHiringCompanies;
    int totalEmployees;
    HighestEarner highestEarner;


public:
    EmployeeManager();
    ~EmployeeManager() = default;
    EmployeeManager(const EmployeeManager& other) = delete;
    EmployeeManager& operator=(const EmployeeManager& other) = delete;

    StatusType AddCompany(int CompanyID, int Value);

    StatusType AddEmployee(int EmployeeID, int CompanyID, int Salary, int Grade);

    StatusType RemoveEmployee(int EmployeeID);

    StatusType RemoveCompany(int CompanyID);

    StatusType GetCompanyInfo(int CompanyID, int* Value, int* NumOfEmployees);

    StatusType GetEmployeeInfo(int EmployeeID, int* EmployerID, int* Salary, int* Grade);

    StatusType IncreaseCompanyValue(int CompanyID, int ValueIncrease);

    StatusType PromoteEmployee(int EmployeeID, int SalaryIncrease, int BumpGrade);

    StatusType HireEmployee(int EmployeeID, int NewCompanyID);

    StatusType AcquireCompany(int AcquirerID, int TargetID, double Factor);

    StatusType GetHighestEarner(int CompanyID, int* EmployeeID);

    StatusType GetAllEmployeesBySalary(int CompanyID, int **Employees, int *NumOfEmployees);

    StatusType GetHighestEarnerInEachCompany(int NumOfCompanies, int **Employees);

    StatusType GetNumEmployeesMatching(int CompanyID, int MinEmployeeID, int MaxEmployeeID, int MinSalary,
                                       int MinGrade, int* TotalNumOfEmployees, int* NumOfEmployees);
};





#endif //TREE2NDATTEMPT_EMPLOYEEMANAGER_H
