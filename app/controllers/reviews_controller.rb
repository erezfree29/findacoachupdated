class ReviewsController < ApplicationController
  def create
    @trainer = Trainer.find(params[:trainer_id])
    @booking= Booking.find(params[:booking_id])
    @review = Review.new(review_params)
    @review.booking_id = @booking.id
    @review.score = params[:data_value]
    @review.save

  end

  def new
    @trainer = Trainer.find(params[:trainer_id])
    @booking= Booking.find(params[:booking_id])
    @review = Review.new()
    @review.score = params[:data_value]
    @review.save

  end

  def show
  end

  def index
  end


  def review_params


  params.require(:review).permit(:title,:content)

 end


end
