// src/middleware/errorHandler.js

//  Global error-handling middleware for Express.
//  Catches any errors passed via next(err) and formats the response.
module.exports = (err, req, res, next) => {
    // Log full error for internal debugging (can be replaced with Winston, Bunyan etc.)
    console.error(err.stack);
  
    // Default status code and message
    const statusCode = err.status || err.statusCode || 500;
    const message = err.message || 'Internal Server Error';
  
    // In production, you might hide stack traces
    const response = {
      error: message,
    };
  
    // Include validation details if available (e.g., from Mongoose)
    if (err.name === 'ValidationError') {
      res.status(400);
      response.details = Object.values(err.errors).map(e => e.message);
    } else {
      res.status(statusCode);
    }
  
    // Optionally include stack trace in non-production
    if (process.env.NODE_ENV !== 'production') {
      response.stack = err.stack;
    }
  
    res.json(response);
  };