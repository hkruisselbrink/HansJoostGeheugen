#ifndef BESTFIT_H
#define BESTFIT_H


class BestFit : public Fitter
{
    public:
        BestFit(bool cflag, const char *type = "BestFit (lazy)")
        : Fitted(cflag, type) {}

        ~BestFit();

        /// Ask for an area of at least 'wanted' units.
        /// @returns	An area or 0 if not enough freespace available
        virtual  Area	*alloc(int wanted);	// application asks for space

        /// The application returns an area to freespace.
        /// @param ap	The area returned to free space
        virtual  void	free(Area *ap);

    protected:

        /// List of all the available free areas
        std::list<Area*>  areas;

        /// For debugging this function shows the free area list
        virtual	 void	dump();

        Area 	*searcher(int);

        virtual  void	updateStats();	///< update resource map statistics
};

#endif // BESTFIT_H
