class ReviewsController < ApplicationController
  def create
    @trainer = Trainer.find(params[:trainer_id])
    @booking= Booking.find(params[:booking_id])
    @review = Review.new(review_params)
    @review.booking_id = @booking.id
    @review.save

    if @review.save

      redirect_to trainer_review_path(@trainer,@review)


    else

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
  end

  def index
  end


  def review_params


  params.require(:review).permit(:title,:content,:score)

 end


end

