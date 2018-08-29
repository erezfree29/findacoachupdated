class ReviewsController < ApplicationController
  def create

    @review = Review.new(review_params)

  end

  def new
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
