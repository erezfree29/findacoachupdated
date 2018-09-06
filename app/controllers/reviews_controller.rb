class ReviewsController < ApplicationController
  def create
    @trainer = Trainer.find(params[:trainer_id])
    @booking= Booking.find(params[:booking_id])
    @review = Review.new(review_params)
    @review.booking_id = @booking.id
    @review.save

    if @review.save

     redirect_to trainers_path


    else

    if @review.id == nil

      flash.now[:alert] = 'you already gave a review'


     elsif @review.score == nil

        flash.now[:alert] = 'please enter a ranking'
    end

     if @review.title == ""

      if @review.score == nil

       flash.now[:alert] << '-please enter a title '

      else

        flash.now[:alert] = 'please enter a title'

     end

     if @review.content == ""

     if @review.score == nil

       flash.now[:alert] << '-please enter your review'

      elsif @review.title == ""

        flash.now[:alert] << '-please enter your review'

      else

         flash.now[:alert] = '-please enter your review'

      end


     end

     end


     render :new

  end

end

  def new
    @trainer = Trainer.find(params[:trainer_id])
    @booking= Booking.find(params[:booking_id])
    @review = Review.new()
    @review.save

  end

  def show

    @trainer = Trainer.find(params[:trainer_id])
    @bookings = Booking.all
    @trainer_bookings = []

    @bookings.each do |booking|

     if (booking.trainer_id == @trainer.id)

          @trainer_bookings << booking

     end

    end

    @reviews = Review.all
    @trainer_reviews = []

    @reviews.each do |review|

    @trainer_bookings.each do |booking|

      if (review.booking_id == booking.id)
         @trainer_reviews << review
      end

    end
   end
  end

  def index

  end


  def review_params


  params.require(:review).permit(:title,:content,:score)

 end


end

