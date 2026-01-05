#ifndef CAMILLE_INCLUDE_CAMILLE_REFACTOR_H_
#define CAMILLE_INCLUDE_CAMILLE_REFACTOR_H_

#endif

/**
 * @brief Gemini Generated Template
 * @brief Multi-threaded Io Contextes

#include <boost/asio.hpp>
#include <vector>
#include <thread>
#include <memory>

class IOContextPool {
public:
    explicit IOContextPool(std::size_t pool_size) : next_io_context_(0) {
        for (std::size_t i = 0; i < pool_size; ++i) {
            // Each worker gets its own context and a 'work_guard'
            // to keep the loop running even if there are no active events.
            auto ctx = std::make_shared<boost::asio::io_context>();
            auto work = boost::asio::make_work_guard(*ctx);

            io_contexts_.push_back(ctx);
            work_guards_.push_back(work);
        }
    }

    void run() {
        std::vector<std::thread> threads;
        for (auto& ctx : io_contexts_) {
            // Start each io_context in its own dedicated thread
            threads.emplace_back([ctx]() { ctx->run(); });
        }
        for (auto& t : threads) t.join();
    }

    // Round-robin selection of a context
    boost::asio::io_context& get_io_context() {
        auto& ctx = *io_contexts_[next_io_context_];
        next_io_context_ = (next_io_context_ + 1) % io_contexts_.size();
        return ctx;
    }

private:
    std::vector<std::shared_ptr<boost::asio::io_context>> io_contexts_;
    std::vector<boost::asio::executor_work_guard<boost::asio::io_context::executor_type>>
work_guards_; std::size_t next_io_context_;
};

class Server {
public:
    Server(short port, std::size_t pool_size)
        : pool_(pool_size),
          acceptor_(pool_.get_io_context(), tcp::endpoint(tcp::v4(), port)) {
        start_accept();
    }

    void run() { pool_.run(); }

private:
    void start_accept() {
        // Get a DIFFERENT io_context for the next client
        auto& client_ctx = pool_.get_io_context();
        auto new_socket = std::make_shared<tcp::socket>(client_ctx);

        acceptor_.async_accept(*new_socket, [this, new_socket](auto ec) {
            if (!ec) {
                // This 'Session' will now be managed by the thread
                // associated with client_ctx.
                std::make_shared<Session>(std::move(*new_socket))->start();
            }
            start_accept();
        });
    }

    IOContextPool pool_;
    tcp::acceptor acceptor_;
};

The "Executor" Pattern: Sometimes you need to move data back to a specific thread. Asio’s post() or
dispatch() functions allow you to safely send a task to a specific io_context from another thread.
 */