#ifndef __PCPE_UTIL_H__
#define __PCPE_UTIL_H__

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <memory>
#include <array>

namespace pcpe{

typedef std::string Filename;
typedef std::vector<Filename> FilenameList;

class ComSubseq{
public:
    ComSubseq(int x=0,
              int y=0,
              int x_loc=0,
              int y_loc=0,
              int len=6):
                  x_(x), y_(y),
                  x_loc_(x_loc), y_loc_(y_loc),
                  len_(len)
    {
    }

    bool operator<(const ComSubseq& rhs) const
    {
        int c[4] = {x_ - rhs.x_,
                    y_ - rhs.y_,
                    x_loc_ - rhs.x_loc_,
                    y_loc_ - rhs.y_loc_};

        for(std::size_t cidx= 0; cidx< 4; ++cidx){
            if(c[cidx]  != 0){
                return c[cidx] < 0;
            } 
        }
        return false;
    }

    bool operator>(const ComSubseq& rhs) const
    {
        int c[4] = {x_ - rhs.x_,
                    y_ - rhs.y_,
                    x_loc_ - rhs.x_loc_,
                    y_loc_ - rhs.y_loc_};

        for(std::size_t cidx= 0; cidx< 4; ++cidx){
            if(c[cidx]  != 0){
                return c[cidx] > 0;
            } 
        }
        return false;
    }

    bool operator==(const ComSubseq& rhs) const
    {
        return (x_ == rhs.x_) && (y_ == rhs.y_) &&
               (x_loc_ == rhs.x_loc_) && (y_loc_ && rhs.y_loc_);
    }
    
    bool operator>=(const ComSubseq& rhs) const { return !(*this < rhs); }
    bool operator<=(const ComSubseq& rhs) const { return !(*this >= rhs); }
    bool operator!=(const ComSubseq& rhs) const { return !(*this == rhs); }

    inline int getLength(){ return len_; };
    inline void setLength(int len){ len_ = len; };

    bool isContinued(const ComSubseq& rhs){
        return (x_ == rhs.x_) && (y_ == rhs.y_) &&
               (rhs.x_loc_ - x_loc_ == 1)  && (rhs.y_loc_ - y_loc_ == 1);
    }


protected:
    int x_;
    int y_;
    int x_loc_;
    int y_loc_;
    int len_; 
};

class ComSubseqFile{
public:
    static void readFile(const Filename& fn, std::vector<ComSubseq>& com_list);
    static void writeFile(const Filename& fn, std::vector<ComSubseq>& com_list);

    ComSubseqFile(Filename fn):com_list_(),com_list_size_(0), fn_(fn){
    };

    virtual void readSeq(ComSubseq& seq) = 0; 
    virtual void writeSeq(const ComSubseq& seq) = 0; 
    virtual void close() = 0;
    virtual bool is_open() = 0;
    virtual bool eof() = 0;
protected:
    const static std::size_t BUFFER_SIZE = 10000;

    std::array<ComSubseq, BUFFER_SIZE> com_list_;
    std::size_t com_list_size_;
    Filename fn_;
};

class ComSubseqFileReader : public ComSubseqFile{
public:
    ComSubseqFileReader(Filename fn):
        ComSubseqFile(fn),
        infile_(fn_.c_str(), std::ifstream::in | std::ifstream::binary),
        read_buffer_idx_(0){

        read_buffer();

    };

    virtual void readSeq(ComSubseq& seq);
    virtual void writeSeq(const ComSubseq& seq);
    
    virtual void close(){
        infile_.close();
        read_buffer_idx_ = 0;
        com_list_size_ = 0;
    }
    
    virtual bool is_open(){ 
        return infile_.is_open();
    }

    virtual bool eof(){
        return !(infile_.is_open() || read_buffer_idx_ != com_list_size_);
    }
    
    bool operator<(const ComSubseqFileReader& rhs)  const { return com_list_[read_buffer_idx_] < rhs.com_list_[read_buffer_idx_];  }
    bool operator>(const ComSubseqFileReader& rhs)  const { return com_list_[read_buffer_idx_] > rhs.com_list_[read_buffer_idx_];  }
    bool operator==(const ComSubseqFileReader& rhs) const { return com_list_[read_buffer_idx_] == rhs.com_list_[read_buffer_idx_]; }
    bool operator>=(const ComSubseqFileReader& rhs) const { return !(*this < rhs);         }
    bool operator<=(const ComSubseqFileReader& rhs) const { return !(*this >= rhs);        }
    bool operator!=(const ComSubseqFileReader& rhs) const { return !(*this == rhs);        }

protected:
    void read_buffer(); 
    std::ifstream infile_;
    std::size_t read_buffer_idx_;
};

class ComSubseqFileWriter : public ComSubseqFile{
public:
    ComSubseqFileWriter(Filename fn):
        ComSubseqFile(fn),
        outfile_(fn_.c_str(), std::ofstream::out | std::ofstream::binary){
    };

    virtual void readSeq(ComSubseq& seq);
    virtual void writeSeq(const ComSubseq& seq);

    virtual void close(){
        write_buffer();
        outfile_.flush();
        outfile_.close();
    }

    virtual bool is_open(){
        return outfile_.is_open();
    }

    virtual bool eof(){
        return false;
    }
    
protected:
    void write_buffer(); 
    std::ofstream outfile_;
};

} // namespace pcpe

#endif
