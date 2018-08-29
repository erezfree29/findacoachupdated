class ReviewsController < ApplicationController
  def create


    @review = Review.new(review_params)
    @review.score = params[:data_value]


  end

  def new
    @trainer = Trainer.find(params[:trainer_id])
    @booking= Booking.find(params[:booking_id])
    @review = Review.new()

  end

  def show
  end

  def index
  end


  def review_params


  params.require(:review).permit(:booking_id)

 end


end
