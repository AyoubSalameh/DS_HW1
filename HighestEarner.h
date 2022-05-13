#ifndef TREE2NDATTEMPT_HIGHESTEARNER_H
#define TREE2NDATTEMPT_HIGHESTEARNER_H

class HighestEarner
{
private:
    int id;
    int salary;
public:
    explicit HighestEarner(int id = 0, int salary = 0);
    HighestEarner(const HighestEarner& other) = default;
    ~HighestEarner() = default;
    HighestEarner& operator=(const HighestEarner& other) = default;

    int getID() const;
    int getSalary() const;

    void setID(int newID);
    void setSalary(int newSalary);

    void compareAndSetSalaries(int newID, int newSalary);
};


#endif //TREE2NDATTEMPT_HIGHESTEARNER_H
