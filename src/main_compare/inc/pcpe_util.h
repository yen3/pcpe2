#ifndef __PCPE_UTIL_H__
#define __PCPE_UTIL_H__

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <memory>

namespace pcpe{

/*****************************************************************************/
// Typedef  
/*****************************************************************************/
typedef std::string Filename;
typedef std::vector<Filename> FilenameList;

class ComSubseq;
typedef std::vector<ComSubseq> ComSubseqList;

/*****************************************************************************/
// Function prototype 
/*****************************************************************************/
/**
 * @brief  Get file size from a file path
 *         The function would not check the file path is valid. It also does
 *         not throw an exception when the file is not founded.
 *
 * @param fn a valid file path
 *
 * @return file size (unit: byte)
 */
std::size_t get_filesize(const Filename& fn);


/**
 * @brief Generate a temporary filename for saving external sort result
 *
 * @param input_seq_x_fn[in] sequnece filename 
 * @param input_seq_y_fn[in] sequnece filename 
 * @param esort_fn[out]      temporary filename
 */
void get_esort_output_file_name(const Filename& input_seq_x_fn,
                                const Filename& input_seq_y_fn,
                                Filename& esort_fn);

/**
 * @brief Generate a temporary filename prefix for saving the comparsion result
 *        between hash tables 
 *
 * @param input_seq_x_fn[in]  sequnece filename 
 * @param input_seq_y_fn[in]  sequnece filename 
 * @param output_prefix[out]  temporary filename prefix
 */
void get_common_subseq_output_prefix(const Filename& input_seq_x_fn,
                                     const Filename& input_seq_y_fn,
                                     Filename& output_prefix);


/*****************************************************************************/
// Class prototype 
/*****************************************************************************/
class ComSubseq{
public:
    ComSubseq(int x=0,
              int y=0,
              int x_loc=0,
              int y_loc=0,
              int len=6):
                  x_(x), y_(y),
                  x_loc_(x_loc), y_loc_(y_loc),
                  len_(len) {
    }

    bool operator<(const ComSubseq& rhs) const {
        if(x_ == rhs.x_){
            if(y_ == rhs.y_){
                if(x_loc_ == rhs.x_loc_){
                    return y_loc_ < rhs.y_loc_;
                }
                return x_loc_ < rhs.x_loc_;
            }
            return y_ < rhs.y_;
        }
        return x_ < rhs.x_;
    }

    bool operator>(const ComSubseq& rhs) const {
        if(x_ == rhs.x_){
            if(y_ == rhs.y_){
                if(x_loc_ == rhs.x_loc_){
                    return y_loc_ > rhs.y_loc_;
                }
                return x_loc_ > rhs.x_loc_;
            }
            return y_ > rhs.y_;
        }
        return x_ > rhs.x_;
    }

    inline bool operator==(const ComSubseq& rhs) const {
        return (x_ == rhs.x_) && (y_ == rhs.y_) &&
               (x_loc_ == rhs.x_loc_) && (y_loc_ == rhs.y_loc_);
    }
    
    inline bool operator>=(const ComSubseq& rhs) const {
        return !(*this < rhs); 
    }
    inline bool operator<=(const ComSubseq& rhs) const {
        return !(*this > rhs);
    }
    inline bool operator!=(const ComSubseq& rhs) const {
        return !(*this == rhs);
    }

    inline int getLength() const { return len_; };
    inline void setLength(int len){ len_ = len; };

    inline bool isContinued(const ComSubseq& rhs) const {
        return (x_ == rhs.x_) && (y_ == rhs.y_) &&
               ((rhs.x_loc_ - x_loc_ == 1)  || (x_loc_ - rhs.x_loc_ == 1)) &&
               ((rhs.y_loc_ - y_loc_ == 1)  || (y_loc_ - rhs.y_loc_ == 1));
    }

    inline bool isSameSeqeunce(const ComSubseq& rhs) const {
        return (x_ == rhs.x_) && (y_ == rhs.y_); 
    }

#if defined(__DEBUG__)
    inline void print(){
        std::cout << "(" << x_ << ", " << y_ << ", " << x_loc_ << ", "
                  << y_loc_ << ", " << len_ << ")" << std::endl;
    }
#endif


protected:
    int x_;
    int y_;
    int x_loc_;
    int y_loc_;
    int len_; 
};

class ComSubseqFileReader{
#if !defined(__GTEST_PCPE__)
    static const std::size_t kInitalReadBufferSize = 10000;
#else
    static const std::size_t kInitalReadBufferSize = 2;
#endif
public:
    static void readFile(const Filename& fn,
             std::vector<ComSubseq>& com_list,
             std::size_t buffer_size=kInitalReadBufferSize);

    static bool esortMergeCompare(const std::shared_ptr<ComSubseqFileReader> x,
                                  const std::shared_ptr<ComSubseqFileReader> y);

    ComSubseqFileReader() = default;
    ComSubseqFileReader(Filename fn,
            std::size_t buffer_size=kInitalReadBufferSize):
        com_list_(buffer_size),
        com_list_size_(0),
        infile_(fn.c_str(), std::ifstream::in | std::ifstream::binary),
        read_buffer_idx_(0),
        file_size_(get_filesize(fn)),
        current_read_file_size_(0)
#if defined(__DEBUG__)
        ,fn_(fn)
#endif
    {

        read_buffer();

    };

    void readSeq(ComSubseq& seq);
    void writeSeq(const ComSubseq& seq);
#if defined(__DEBUG__)
    const Filename& getFn() const { return fn_; };
#endif
    
    inline void close(){
        infile_.close();
    }
    
    inline bool is_open() const { 
        return infile_.is_open() || read_buffer_idx_ < com_list_size_;
    }

    inline bool eof() const {
        if(is_open()) return false;
        else return read_buffer_idx_ >= com_list_size_; 
    }


#if defined(__DEBUG__)
    void print(){
        std::cout << fn_ << ": " << "read_buffer_idx_ : "
                  << read_buffer_idx_ << std::endl;
        for(std::size_t i = 0; i< com_list_size_; ++i){
            std::cout << "\t";
            com_list_[i].print();
        }
    }
#endif

protected:
    ComSubseqFileReader(const ComSubseqFileReader&);
    ComSubseqFileReader(const ComSubseqFileReader&&);
    ComSubseqFileReader& operator=(const ComSubseqFileReader&);
    void read_buffer(); 

    std::vector<ComSubseq> com_list_;
    std::size_t com_list_size_;

    std::ifstream infile_;
    std::size_t read_buffer_idx_;

    std::size_t file_size_;               // unit: byte
    std::size_t current_read_file_size_;  // unit: byte

#if defined(__DEBUG__)
    Filename fn_;
#endif
};

class ComSubseqFileWriter{
#if !defined(__GTEST_PCPE__)
    static const std::size_t kInitialWriteBufferSize = 10000;
#else
    static const std::size_t kInitialWriteBufferSize = 2;
#endif
public:
    static void writeFile(const Filename& fn,
                          std::vector<ComSubseq>& com_list,
                          std::size_t buffer_size=kInitialWriteBufferSize);

    ComSubseqFileWriter() = default;
    ComSubseqFileWriter(Filename fn,
            std::size_t buffer_size=kInitialWriteBufferSize):
        com_list_(buffer_size),
        com_list_size_(0),
        outfile_(fn.c_str(), std::ofstream::out | std::ofstream::binary)
#if defined(__DEBUG__)
        ,fn_(fn)
#endif
    {

    }

    ~ComSubseqFileWriter(){
        if(is_open()){
            close();
        }
    }

    void readSeq(ComSubseq& seq);
    void writeSeq(const ComSubseq& seq);
#if defined(__DEBUG__)
    const Filename& getFn(){ return fn_;};
#endif

    inline void close(){
        write_buffer();
        outfile_.flush();
        outfile_.close();
    }

    inline bool is_open() const { return outfile_.is_open(); }
    
protected:
    ComSubseqFileWriter(const ComSubseqFileWriter&);
    ComSubseqFileWriter(const ComSubseqFileWriter&&);
    ComSubseqFileWriter& operator=(const ComSubseqFileWriter&);

    void write_buffer(); 

    std::vector<ComSubseq> com_list_;
    std::size_t com_list_size_;

    std::ofstream outfile_;

#if defined(__DEBUG__)
    Filename fn_;
#endif
};


} // namespace pcpe

#endif
